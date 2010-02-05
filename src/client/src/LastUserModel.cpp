#include "stdafx.h"
#include "LastUserModel.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

LastUserModel::LastUserModel(IRegistryKey & registryKey)
	: registryKey (registryKey)
{
}

const ICredentialsModel & LastUserModel::GetCredentials()
{
	wstring username = registryKey.GetString(L"username", L"");
	wstring password = registryKey.GetString(L"password", L"");

	credentialsModel.SetCredentials(username, password);
	return credentialsModel;
}