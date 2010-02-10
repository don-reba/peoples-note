#include "stdafx.h"
#include "CurrentUserLoader.h"

using namespace boost;
using namespace std;

CurrentUserLoader::CurrentUserLoader
	( IUserModel     & userModel
	, ILastUserModel & lastUserModel
	, INoteListView  & noteListView
	)
	: userModel     (userModel)
	, lastUserModel (lastUserModel)
	, noteListView  (noteListView)
{
	noteListView.ConnectCreated(bind(&CurrentUserLoader::OnCreated, this));
}

void CurrentUserLoader::OnCreated()
{
	const ICredentialsModel & credentials = lastUserModel.GetCredentials();
	if (credentials.GetUsername().empty())
		userModel.CreateDefaultUser();
	else
		userModel.SetCredentials(credentials);
	userModel.Load();
}
