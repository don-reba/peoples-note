#include "stdafx.h"
#include "UserSignInPresenter.h"

#include "ICredentialsModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

using namespace boost;

UserSignInPresenter::UserSignInPresenter
	( INoteListView & noteListView
	, IUserModel    & userModel
	)
	: noteListView (noteListView)
	, userModel    (userModel)
{
	noteListView.ConnectSignIn(bind(&UserSignInPresenter::OnSignIn, this));
	userModel.GetCredentials().ConnectUpdated(bind(&UserSignInPresenter::OnCredentialsUpdated, this));
}

void UserSignInPresenter::OnCredentialsUpdated()
{
	if (!userModel.GetCredentials().GetUsername().empty())
		noteListView.SignIn();
}

void UserSignInPresenter::OnSignIn()
{
	userModel.GetCredentials().Update();
}
