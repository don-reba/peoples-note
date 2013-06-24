#include "stdafx.h"
#include "UserLoader.h"

#include "ICredentialsModel.h"
#include "ILastUserModel.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

UserLoader::UserLoader
	( ICredentialsModel & credentialsModel
	, ILastUserModel    & lastUserModel
	)
	: credentialsModel (credentialsModel)
	, lastUserModel    (lastUserModel)
{
	credentialsModel.ConnectCommit(bind(&UserLoader::OnCommit, this));
}

void UserLoader::Run()
{
	credentialsModel.Set
		( lastUserModel.GetUsername()
		, lastUserModel.GetToken()
		, lastUserModel.GetShard()
		);
}

void UserLoader::OnCommit()
{
	lastUserModel.SetCredentials
		( credentialsModel.GetUsername()
		, credentialsModel.GetToken()
		, credentialsModel.GetShard()
		);
}
