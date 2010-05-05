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
	{
		userModel.LoadOrCreate(L"[anonymous]");
	}
	else
	{
		try
		{
			userModel.Load(username);
		}
		catch (const std::exception &)
		{
			userModel.LoadOrCreate(L"[anonymous]");
		}
	}
}

void UserLoader::Save()
{
	Credentials credentials;
	userModel.GetCredentials(credentials);
	lastUserModel.SetUsername(credentials.GetUsername());
}
