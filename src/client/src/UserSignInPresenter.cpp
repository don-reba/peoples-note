#include "stdafx.h"
#include "UserSignInPresenter.h"

#include "ICredentialsModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

UserSignInPresenter::UserSignInPresenter
	( ICredentialsModel & newCredentials
	, INoteListView     & noteListView
	, IUserModel        & userModel
	)
	: newCredentials (newCredentials)
	, noteListView   (noteListView)
	, userModel      (userModel)
{
	noteListView.ConnectSignIn(bind(&UserSignInPresenter::OnSignIn, this));
	newCredentials.ConnectUpdated(bind(&UserSignInPresenter::OnCredentialsUpdated, this));
}

void UserSignInPresenter::OnCredentialsUpdated()
{
	const wstring & username = newCredentials.GetUsername();
	if (username.empty())
		return;
	userModel.Unload();
	if (userModel.Exists(username))
		userModel.Load(username);
	else
		userModel.LoadAs(L"[anonymous]", username);
}

void UserSignInPresenter::OnSignIn()
{
	newCredentials.Update();
}
