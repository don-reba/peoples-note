#include "stdafx.h"
#include "CurrentUserLoader.h"

using namespace boost;
using namespace std;

CurrentUserLoader::CurrentUserLoader
	( IAppModel      & appModel
	, IUserModel     & userModel
	, ILastUserModel & lastUserModel
	)
	: appModel      (appModel)
	, userModel     (userModel)
	, lastUserModel (lastUserModel)
{
	appModel.ConnectStart(bind(&CurrentUserLoader::OnStart, this));
}

void CurrentUserLoader::OnStart()
{
	const ICredentialsModel & credentials = lastUserModel.GetCredentials();
	if (credentials.GetUsername().empty())
		userModel.CreateDefaultUser();
	else
		userModel.SetCredentials(credentials);
	userModel.Load();
}
