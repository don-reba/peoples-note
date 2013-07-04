#include "stdafx.h"
#include "UserSignInPresenter.h"

#include "ICredentialsModel.h"
#include "IEnService.h"
#include "INoteListView.h"
#include "IUserModel.h"
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
	userModel.Unload();

	wstring username (credentialsModel.GetUsername());
	wstring token    (credentialsModel.GetToken());
	wstring shard    (credentialsModel.GetShard());

	if (username.empty())
	{
		credentialsModel.Set(anonymousUser, L"", L"");
		return;
	}

	if (username == anonymousUser)
	{
		userModel.LoadOrCreate(username);
		credentialsModel.Commit();
		return;
	}

	if (token.empty())
	{
		credentialsModel.Update(L"Please, enter a developer token.");
		return;
	}

	if (shard.empty())
	{
		credentialsModel.Update(L"Please, enter a notestore url.");
		return;
	}

	if (userModel.Exists(username))
	{
		userModel.Load(username);

		credentialsModel.Commit();
	}
	else
	{
		userModel.LoadAs(anonymousUser, username);
		userModel.SetCredentials
			( username
			, HashPassword(token)
			);
		credentialsModel.Commit();
	}
}
catch (const std::exception & e)
{
	credentialsModel.Update(ConvertToUnicode(e.what()).c_str());
}

void UserSignInPresenter::OnSignIn()
{
	if (userModel.GetUsername() == anonymousUser)
		credentialsModel.Update(L"");
	else
		credentialsModel.Set(anonymousUser, L"", L"");
}
