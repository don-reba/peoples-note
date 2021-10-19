#include "stdafx.h"
#include "UserStore.h"

#include "IUserModel.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/UserStore.h"

#include "API Key.h"
#include "ILogger.h"
#include "Tools.h"
#include "Transaction.h"

using namespace std;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote;

UserStore::UserStore(ILogger & logger)
	: logger (logger)
{
}

IUserStore::AuthenticationResult UserStore::GetAuthenticationToken
	( wstring username
	, wstring password
	)
{
	AuthenticationResult result;
	result.IsGood  = true;
	result.Message = L"";
	result.ShardId = L"https://www.evernote.com/shard/s9/notestore";
	result.Token   = L"S=s9:U=f0333:E=14659dc23fc:C=13f022af800:P=1cd:A=en-devtoken:V=2:H=fdb22ec26cb9c45cf3d77e243675002e";
	return result;
}
/*
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
	catch (const Thrift::Transport::TTransportException &)
	{
		logger.AuthorizationError(username, logger.GetExceptionMessage().Message);
		result.Message = L"Network error";
	}
	catch (const EDAM::Error::EDAMUserException &)
	{
		logger.AuthorizationError(username, logger.GetExceptionMessage().Message);
		result.Message = L"Invalid username or password";
	}
	catch (const TException &)
	{
		logger.AuthorizationError(username, logger.GetExceptionMessage().Message);
		result.Message = L"Something went wrong";
	}
	return result;
}
*/