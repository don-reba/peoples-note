#include "stdafx.h"
#include "LastUserModel.h"

#include "IRegistryKey.h"
#include "Tools.h"

using namespace std;

LastUserModel::LastUserModel(IRegistryKey & registryKey)
	: registryKey (registryKey)
{
}

wstring LastUserModel::GetPassword() const
{
	return registryKey.GetString(L"password", L"");
}

wstring LastUserModel::GetUsername() const
{
	return registryKey.GetString(L"username", L"");
}

void LastUserModel::SetCredentials
	( const std::wstring & username
	, const std::wstring & password
	)
{
	registryKey.SetString(L"username", username.c_str());
	registryKey.SetString(L"password", password.c_str());
}
