#include "stdafx.h"
#include "UserSignInPresenter.h"

#include "ICredentialsModel.h"
#include "IEnService.h"
#include "INoteListView.h"
#include "IUserModel.h"
#include "IUserStore.h"
#include "Tools.h"
#include "Transaction.h"

using namespace boost;
using namespace std;
using namespace Tools;

UserSignInPresenter::UserSignInPresenter
	( ICredentialsModel & credentialsModel
	, IEnService        & enService
	, INoteListView     & noteListView
	, IUserModel        & userModel
	)
	: credentialsModel (credentialsModel)
	, enService        (enService)
	, noteListView     (noteListView)
	, userModel        (userModel)
{
	noteListView.ConnectSignIn(bind(&UserSignInPresenter::OnSignIn, this));
	credentialsModel.ConnectSet(bind(&UserSignInPresenter::OnCredentialsSet, this));
}

void UserSignInPresenter::OnCredentialsSet()
try
{
	wstring username(credentialsModel.GetUsername());
	wstring password(credentialsModel.GetPassword());

	if (username.empty())
	{
		credentialsModel.Update(L"Please, enter a username.");
		return;
	}

	userModel.Unload();

	if (username == L"[anonymous]")
	{
		userModel.LoadOrCreate(username);
		credentialsModel.Commit();
		return;
	}

	if (password.empty())
	{
		credentialsModel.Update(L"Please, enter a password.");
		return;
	}

	if (userModel.Exists(username))
	{
		userModel.Load(username);

		Credentials credentials;
		userModel.GetCredentials(credentials);

		if (HashPassword(password) == credentials.Password)
		{
			credentialsModel.Commit();
		}
		else
		{
			userModel.Unload();
			credentialsModel.Update(L"The password is invalid.");
		}
	}
	else
	{
		IUserStore::AuthenticationResult authenticationResult =
			enService.GetUserStore()->GetAuthenticationToken(username, password);
		if (authenticationResult.IsGood)
		{
			userModel.LoadAs(L"[anonymous]", username);
			userModel.SetCredentials
				( username
				, HashPassword(credentialsModel.GetPassword())
				);
			credentialsModel.Commit();
		}
		else
		{
			credentialsModel.Update(authenticationResult.Message.c_str());
		}
	}
}
catch (const std::exception & e)
{
	credentialsModel.Update(ConvertToUnicode(e.what()).c_str());
}

void UserSignInPresenter::OnSignIn()
{
	//Credentials credentials;
	//userModel.GetCredentials(credentials);
	//if (credentials.Username == L"[anonymous]")
	//	credentialsModel.SetStatus(L"");
	//else
	//	credentialsModel.SetCredentials(L"[anonymous]", L"");
}
