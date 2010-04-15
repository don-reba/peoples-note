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
	const ICredentialsModel & credentials(lastUserModel.GetCredentials());

	if (credentials.GetUsername().empty())
		userModel.LoadOrCreate(L"[anonymous]");
	else
		userModel.Load(credentials.GetUsername());
}
