#include "stdafx.h"
#include "THttpTransport.h"

#include "../Protocol/TBinary.h"
#include "../Protocol/TString.h"
#include "TTransportException.h"

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <vector>

using namespace std;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

int dbgCounter = 1;

THttpTransport::THttpTransport(LPCWSTR url)
	: url            (url)
	, requestHandle  (NULL)
	, sessionHandle  (NULL)
	, internetHandle (NULL)
{
}

THttpTransport::~THttpTransport()
{
	Close();
}

//--------------------------
// TTransport implementation
//--------------------------

bool THttpTransport::IsOpen()
{
	return NULL != sessionHandle;
}

void THttpTransport::Open()
{
	if (IsOpen())
		throw TTransportException(TTransportException::AlreadyOpen, L"Open failed.");

	wstring serverName;

	ParseUrl(url, isConnectionSecure, serverName, relativeUrl);

	internetHandle = ConnectToInternet
		( L"WinC++/THttpTransport"
		);
	sessionHandle = BeginSession
		( internetHandle
		, serverName.c_str()
		, isConnectionSecure
		);
}

void THttpTransport::Close()
{
	CloseHandle(requestHandle);
	CloseHandle(sessionHandle);
	CloseHandle(internetHandle);
}

int THttpTransport::Read
	( TBinary & buf
	, int       off
	, int       len
	)
{
	if (!IsOpen())
		throw TTransportException(TTransportException::NotOpen, L"Read failed");

	if (!buffer.empty())
		Flush();

	DWORD bytesRead(0);
	BOOL result = ::InternetReadFile
		( requestHandle // hFile
		, &buf.at(off)  // lpBuffer
		, len           // dwNumberOfBytesToRead
		, &bytesRead    // lpdwNumberOfBytesRead
		);
	if (!result)
		throw TTransportException(TTransportException::Unknown, L"Read failed.");

	return bytesRead;
}

void THttpTransport::Write
	( const TBinary & buf
	, int off
	, int len
	)
{
	if (!IsOpen())
		throw TTransportException(TTransportException::NotOpen, L"Write failed");
	buffer.reserve(buffer.size() + buf.size());
	copy
		( buf.begin() + off
		, buf.begin() + off + len
		, back_inserter(buffer)
		);
}

void THttpTransport::Flush()
{
	CloseHandle(requestHandle);
	requestHandle = OpenRequest
		( sessionHandle
		, relativeUrl.c_str()
		, isConnectionSecure
		);
	SendRequest(requestHandle, buffer);
	buffer.clear();
}

//------------------
// utility functions
//------------------

HINTERNET THttpTransport::BeginSession
	( HINTERNET internetHandle
	, LPCWSTR   serverName
	, bool      isConnectionSecure
	)
{
	const INTERNET_PORT serverPort
		= isConnectionSecure
		? INTERNET_DEFAULT_HTTPS_PORT
		: INTERNET_DEFAULT_HTTP_PORT
		;
	HINTERNET sessionHandle = ::InternetConnect
		( internetHandle        // hInternet
		, serverName            // lpszServername
		, serverPort            // nServerPort
		, L""                   // lpszUserName
		, L""                   // lpszPassword
		, INTERNET_SERVICE_HTTP // dwService
		, 0                     // dwFlags
		, 0                     // dwContext
		);
	if (NULL == sessionHandle)
		throw TTransportException(TTransportException::Unknown, L"Coult not begin session.");
	return sessionHandle;
}

void THttpTransport::CloseHandle(HINTERNET & handle)
{
	if (NULL == handle)
		return;
	::InternetCloseHandle(handle);
	handle = NULL;
}

HINTERNET THttpTransport::ConnectToInternet(LPCWSTR agent)
{
	HINTERNET sessionHandle = InternetOpen
		( agent                        // lpszAgent
		, INTERNET_OPEN_TYPE_PRECONFIG // dwAccessType
		, NULL                         // lpszProxy
		, NULL                         // lpszProxyBypass
		, 0                            // dwFlags
		);
	if (NULL == sessionHandle)
		throw TTransportException(TTransportException::Unknown, L"Coult not establish internet connection.");

	unsigned long connectTimeout(8000);
	::InternetSetOption
		( sessionHandle
		, INTERNET_OPTION_CONNECT_TIMEOUT
		, &connectTimeout
		, sizeof(connectTimeout)
		);
	unsigned long receiveTimeout(8000);
	::InternetSetOption
		( sessionHandle
		, INTERNET_OPTION_RECEIVE_TIMEOUT
		, &receiveTimeout
		, sizeof(receiveTimeout)
		);
	unsigned long sendTimeout(8000);
	::InternetSetOption
		( sessionHandle
		, INTERNET_OPTION_SEND_TIMEOUT 
		, &sendTimeout
		, sizeof(sendTimeout)
		);

	return sessionHandle;
}

HINTERNET THttpTransport::OpenRequest
	( HINTERNET sessionHandle
	, LPCWSTR   relativeUrl
	, bool      isConnectionSecure
	)
{
	const wchar_t * acceptTypes[] = { L"application/x-thrift", NULL };
	const DWORD flags
		= isConnectionSecure
		? INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE
		: INTERNET_FLAG_NO_UI
		;
	HINTERNET requestHandle = ::HttpOpenRequest
		( sessionHandle // hConnect
		, L"POST"       // lpszVerb
		, relativeUrl   // lpszObjectName
		, NULL          // lpszVersion
		, NULL          // lpszReferrer
		, acceptTypes   // lplpszAcceptTypes
		, flags         // dwFlags
		, 0             // dwContext
		);
	if (NULL == requestHandle)
		throw TTransportException(TTransportException::Unknown, L"Could not connect to url.");
	return requestHandle;
}

void THttpTransport::ParseUrl
	( const wstring & url
	, bool          & isConnectionSecure
	, wstring       & serverName
	, wstring       & relativeUrl
	)
{
	vector<wchar_t> serverNameChars  (url.size() + 1);
	vector<wchar_t> relativeUrlChars (url.size() + 1);

	URL_COMPONENTS urlComponents = { sizeof(urlComponents) };
	urlComponents.lpszHostName     = &serverNameChars[0];
	urlComponents.dwHostNameLength = serverNameChars.size();
	urlComponents.lpszUrlPath      = &relativeUrlChars[0];
	urlComponents.dwUrlPathLength  = relativeUrlChars.size();
	::InternetCrackUrl
		( url.c_str()    // lpszUrl
		, url.size()     // dwUrlLength
		, ICU_DECODE     // dwFlags
		, &urlComponents // lpUrlComponents
		);

	serverName  = &serverNameChars[0];
	relativeUrl = &relativeUrlChars[0];

	// check protocol
	switch (urlComponents.nScheme)
	{
	case INTERNET_SCHEME_HTTP:  isConnectionSecure = false; break;
	case INTERNET_SCHEME_HTTPS: isConnectionSecure = true;  break;
	default:
		throw TTransportException(TTransportException::AlreadyOpen, L"Invalid protocol.");
	}
}

void THttpTransport::SendRequest
	( HINTERNET      requestHandle
	, vector<BYTE> & data
	)
{
	if (data.empty())
		return;

	static wstring headers(L"Content-Type: application/x-thrift\r\n");

	BOOL success = ::HttpSendRequest
		( requestHandle   // hRequest
		, headers.c_str() // lpszHeaders
		, headers.size()  // dwHedersLength
		, &data[0]        // lpOptional
		, data.size()     // dwOptionalLength
		);
	if (!success)
	{
		DWORD   error  (::GetLastError());
		DWORD   flags  (FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_IGNORE_INSERTS);
		HMODULE source (GetModuleHandle(L"wininet.dll"));
		vector<wchar_t> buffer(100);
		::FormatMessage
			( flags         // dwFlags
			, source        // lpSource
			, error         // dwMessageId
			, 0             // dwLanguageId
			, &buffer[0]    // pBuffer
			, buffer.size() // nSize
			, NULL          // Arguments
			);
		throw TTransportException
			( TTransportException::Unknown
			, buffer[0]
			? &buffer[0]
			: L"Request could not be sent."
			);
	}
}
