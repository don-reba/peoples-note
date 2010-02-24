#include "stdafx.h"
#include "UserLoader.h"

#include "ICredentialsModel.h"
#include "ILastUserModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

UserLoader::UserLoader
	( IUserModel     & userModel
	, ILastUserModel & lastUserModel
	, INoteListView  & noteListView
	)
	: userModel     (userModel)
	, lastUserModel (lastUserModel)
	, noteListView  (noteListView)
{
	noteListView.ConnectCreated(bind(&UserLoader::OnCreated, this));
}

void UserLoader::OnCreated()
{
	const ICredentialsModel & credentials = lastUserModel.GetCredentials();
	if (credentials.GetUsername().empty())
		userModel.CreateDefaultUser();
	else
		userModel.SetCredentials(credentials);
	userModel.Load();
}
