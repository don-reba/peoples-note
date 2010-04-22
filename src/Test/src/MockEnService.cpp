#include "stdafx.h"
#include "MockEnService.h"

#include "IUserModel.h"

using namespace std;

MockEnService::MockEnService()
	: hasSynced (false)
{
	credentialsValidity.IsGood  = false;
	credentialsValidity.Message = L"";
}

IEnService::CredentialsValidity MockEnService::CheckCredentials
	( const wstring & username
	, const wstring & password
	)
{
	this->username = username;
	this->password = password;
	return credentialsValidity;
}

void MockEnService::Sync(IUserModel & userModel)
{
	hasSynced = true;
}
