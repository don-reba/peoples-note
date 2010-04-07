#include "stdafx.h"
#include "MockEnService.h"

using namespace std;

MockEnService::MockEnService()
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
