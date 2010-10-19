#include "stdafx.h"
#include "LastUserModel.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

LastUserModel::LastUserModel(IRegistryKey & registryKey)
	: registryKey (registryKey)
{
}
wstring LastUserModel::GetUsername()
{
	return registryKey.GetString(L"username", L"");
}

void LastUserModel::SetUsername(const std::wstring & username)
{
	registryKey.SetString(L"username", username.c_str());
}
