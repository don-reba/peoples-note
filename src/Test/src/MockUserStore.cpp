#include "stdafx.h"
#include "MockUserStore.h"

using namespace std;

IUserStore::AuthenticationResult MockUserStore::GetAuthenticationToken
	( wstring username
	, wstring password
	)
{
	return authenticationResult;
}
