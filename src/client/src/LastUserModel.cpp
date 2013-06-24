#include "stdafx.h"
#include "LastUserModel.h"

#include "IRegistryKey.h"
#include "Tools.h"

using namespace std;

LastUserModel::LastUserModel(IRegistryKey & registryKey)
	: registryKey (registryKey)
{
}

wstring LastUserModel::GetShard() const
{
	return registryKey.GetString(L"shard", L"");
}

wstring LastUserModel::GetToken() const
{
	return registryKey.GetString(L"token", L"");
}

wstring LastUserModel::GetUsername() const
{
	return registryKey.GetString(L"username", L"");
}

void LastUserModel::SetCredentials
	( const std::wstring & username
	, const std::wstring & token
	, const std::wstring & shard
	)
{
	registryKey.SetString(L"username", username.c_str());
	registryKey.SetString(L"token",    token.c_str());
	registryKey.SetString(L"shard",    shard.c_str());
}
