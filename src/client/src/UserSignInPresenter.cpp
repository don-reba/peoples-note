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

const wstring anonymousUser(L"[anonymous]");

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
		credentialsModel.Update(L"");
		return;
	}

	userModel.Unload();

	if (username == anonymousUser)
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

		if (HashPassword(password) == userModel.GetPasswordHash())
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
			userModel.LoadAs(anonymousUser, username);
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
	if (userModel.GetUsername() == anonymousUser)
		credentialsModel.Set(L"", L"");
	else
		credentialsModel.Set(anonymousUser, L"");
}
