#include "stdafx.h"
#include "UserLoader.h"

#include "ICredentialsModel.h"
#include "ILastUserModel.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

UserLoader::UserLoader
	( IUserModel     & userModel
	, ILastUserModel & lastUserModel
	)
	: userModel     (userModel)
	, lastUserModel (lastUserModel)
{
}

void UserLoader::Run()
{
	const wstring & username(lastUserModel.GetUsername());
	if (username.empty())
		userModel.LoadOrCreate(L"[anonymous]");
	else
		userModel.Load(username);
}

void UserLoader::Save()
{
	lastUserModel.SetUsername(userModel.GetCredentials().GetUsername());
}
