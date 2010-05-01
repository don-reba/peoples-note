#include "stdafx.h"

#include "CredentialsPresenter.h"
#include "ICredentialsModel.h"
#include "ICredentialsView.h"
#include "IEnService.h"
#include "IUserStore.h"

using namespace boost;
using namespace std;

CredentialsPresenter::CredentialsPresenter
	( ICredentialsModel & credentialsModel
	, ICredentialsView  & credentialsView
	, IEnService        & enService
	)
	: credentialsModel (credentialsModel)
	, credentialsView  (credentialsView)
	, enService        (enService)
{
	credentialsModel.ConnectUpdating (bind(&CredentialsPresenter::OnModelUpdating, this));
	credentialsView.ConnectCreated   (bind(&CredentialsPresenter::OnViewCreated,   this));
	credentialsView.ConnectOk        (bind(&CredentialsPresenter::OnOk,            this));
	credentialsView.ConnectCancel    (bind(&CredentialsPresenter::OnCancel,        this));
}

void CredentialsPresenter::OnModelUpdating()
{
	credentialsView.Open();
}

void CredentialsPresenter::OnViewCreated()
{
	credentialsView.SetUsername(credentialsModel.GetUsername());
	credentialsView.SetPassword(credentialsModel.GetPassword());
}

void CredentialsPresenter::OnOk()
{
	wstring username(credentialsView.GetUsername());
	wstring password(credentialsView.GetPassword());

	if (username == L"[anonymous]")
	{
		credentialsView.SetMessage(L"Invalid username.");
	}
	else if (username.empty())
	{
		credentialsView.SetMessage(L"Please, enter a username.");
	}
	else if (password.empty())
	{
		credentialsView.SetMessage(L"Please, enter a password.");
	}
	else
	{
		IUserStore::AuthenticationResult authenticationResult =
			enService.GetUserStore()->GetAuthenticationToken(username, password);
		if (authenticationResult.IsGood)
		{
			credentialsModel.SetCredentials(username, password);
			credentialsView.Close();
		}
		else
		{
			wstring emptyPassword(L"");
			credentialsView.SetPassword(emptyPassword);
			credentialsView.SetMessage(authenticationResult.Message);
		}
	}
}

void CredentialsPresenter::OnCancel()
{
	credentialsView.Close();
}
