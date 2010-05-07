#include "stdafx.h"
#include "UserStore.h"

#include "IUserModel.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/NoteStore.h"
#include "SyncLogic.h"
#include "Tools.h"
#include "Transaction.h"

using namespace std;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote;

IUserStore::AuthenticationResult UserStore::GetAuthenticationToken
	( wstring username
	, wstring password
	)
{
	const wchar_t * consumerKey      = L"donreba";
	const wchar_t * consumerSecret   = L"3d764d03e2b1c7c4";
	const wchar_t * userStoreUrl     = L"https://sandbox.evernote.com/edam/user";
	const wchar_t * noteStoreUrlBase = L"http://sandbox.evernote.com/edam/note/";

	AuthenticationResult result;
	result.IsGood = false;

	try
	{
		THttpTransport  transport (userStoreUrl);
		TBinaryProtocol protocol  (transport);
		transport.Open();

		EDAM::UserStore::UserStore::Client userStore(protocol);

		bool versionOk = userStore.checkVersion
			( L"Peoples' Note"
			, EDAM::UserStore::constants.EDAM_VERSION_MAJOR
			, EDAM::UserStore::constants.EDAM_VERSION_MINOR
			);
		if (!versionOk)
		{
			result.Message = L"Incompatible EDAM client protocol version\n";
			return result;
		}

		EDAM::UserStore::AuthenticationResult authenticationResult
			= userStore.authenticate
			( username
			, password
			, consumerKey
			, consumerSecret
			);

		result.IsGood = true;
		result.Token = authenticationResult.authenticationToken; 
	}
	catch (const EDAM::Error::EDAMUserException & e)
	{
		result.Message = e.parameter;
	}
	catch (const EDAM::Error::EDAMSystemException & e)
	{
		result.Message = e.message;
	}
	catch (const TException & e)
	{
		result.Message = e.GetMessage();
	}
	return result;
}
