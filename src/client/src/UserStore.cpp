#include "stdafx.h"
#include "UserStore.h"

#include "IUserModel.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/UserStore.h"

#include "API Key.h"
#include "EnServiceTools.h"
#include "SyncLogic.h"
#include "Tools.h"
#include "Transaction.h"

using namespace std;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote;

using namespace EnServiceTools;

IUserStore::AuthenticationResult UserStore::GetAuthenticationToken
	( wstring username
	, wstring password
	)
{
	const wchar_t * userStoreUrl   = L"https://www.evernote.com/edam/user";

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

		result.IsGood  = true;
		result.ShardId = authenticationResult.user.shardId;
		result.Token   = authenticationResult.authenticationToken; 
	}
	catch (const EDAM::Error::EDAMUserException & e)
	{
		result.Message = CreateUserExceptionMessage(e);
	}
	catch (const EDAM::Error::EDAMSystemException & e)
	{
		result.Message = CreateSystemExceptionMessage(e);
	}
	catch (const Thrift::Transport::TTransportException & e)
	{
		result.Message = CreateTransportExceptionMessage(e);
	}
	catch (const TException & e)
	{
		result.Message = CreateExceptionMessage(e);
	}
	return result;
}
