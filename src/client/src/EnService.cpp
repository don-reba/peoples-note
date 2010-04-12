#include "stdafx.h"
#include "EnService.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/NoteStore.h"

using namespace std;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote::EDAM;

IEnService::CredentialsValidity EnService::CheckCredentials
	( const wstring & username
	, const wstring & password
	)
{
	const wchar_t * consumerKey      = L"donreba";
	const wchar_t * consumerSecret   = L"3d764d03e2b1c7c4";
	const wchar_t * userStoreUrl     = L"https://sandbox.evernote.com/edam/user";
	const wchar_t * noteStoreUrlBase = L"http://sandbox.evernote.com/edam/note/";

	try
	{
		THttpTransport  transport (userStoreUrl);
		TBinaryProtocol protocol  (transport);
		transport.Open();

		UserStore::UserStore::Client userStore(protocol);

		bool versionOk = userStore.checkVersion
			( L"Peoples' Note"
			, UserStore::constants.EDAM_VERSION_MAJOR
			, UserStore::constants.EDAM_VERSION_MINOR
			);
		if (!versionOk)
		{
			CredentialsValidity validity;
			validity.IsGood  = false;
			validity.Message = L"Incompatible EDAM client protocol version\n";
			return validity;
		}

		userStore.authenticate
			( username.c_str()
			, password.c_str()
			, consumerKey
			, consumerSecret
			);

		CredentialsValidity validity;
		validity.IsGood = true;
		return validity;
	}
	catch (const TException & e)
	{
		CredentialsValidity validity;
		validity.IsGood  = false;
		validity.Message = e.GetMessage();
		return validity;
	}
}
