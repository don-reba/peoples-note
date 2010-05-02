#include "stdafx.h"
#include "MockUserStore.h"

using namespace std;

MockUserStore::MockUserStore()
{
	authenticationResult.IsGood  = false;
	authenticationResult.Message = L"";
}

IUserStore::AuthenticationResult MockUserStore::GetAuthenticationToken
	( wstring username
	, wstring password
	)
{
	return authenticationResult;
}
