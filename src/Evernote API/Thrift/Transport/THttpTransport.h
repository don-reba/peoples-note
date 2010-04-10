#pragma once

#include "../Protocol/TBinary.h"
#include "TTransport.h"

#include <string>
#include <vector>
#include <wininet.h>

namespace Thrift
{
namespace Transport
{

	class THttpTransport : public TTransport
	{
	// data
	private:

		std::wstring relativeUrl;
		std::wstring url;

		bool isConnectionSecure;

		std::vector<BYTE> buffer;

		HINTERNET requestHandle;
		HINTERNET sessionHandle;
		HINTERNET internetHandle;

	// interface
	public:

		THttpTransport(LPCWSTR url);

		~THttpTransport();

	// TTransport implementation
	public:

		virtual bool IsOpen();

		virtual void Open();

		virtual void Close();

		virtual int Read
			( Protocol::TBinary & buf
			, int                 off
			, int                 len
			);

		virtual void Write
			( const Protocol::TBinary & buf
			,       int                 off
			,       int                 len
			);

		virtual void Flush();

	// utility functions
	private:

		static HINTERNET BeginSession
			( HINTERNET internetHandle
			, LPCWSTR   serverName
			, bool      isConnectionSecure
			);

		static void CloseHandle(HINTERNET & handle);

		static HINTERNET ConnectToInternet(LPCWSTR agent);

		static HINTERNET OpenRequest
			( HINTERNET  sessionHandle
			, LPCWSTR    relativeUrl
			, bool       isConnectionSecure
			);

		static void ParseUrl
			( const std::wstring & url
			,       bool         & isConnectionSecure
			,       std::wstring & serverName
			,       std::wstring & relativeUrl
			);

		static void SendRequest
			( HINTERNET           requestHandle
			, std::vector<BYTE> & data
			);
	};

} // Transport
} // Thrift

