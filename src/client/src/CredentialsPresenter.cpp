#include "stdafx.h"

#include "CredentialsPresenter.h"
#include "ICredentialsModel.h"
#include "ICredentialsView.h"
#include "IEnService.h"
#include "IUserStore.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

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
	wstring username(credentialsModel.GetUsername());
	wstring password(credentialsModel.GetPassword());
	if (username.empty() || username == L"[anonymous]")
	{
		username = L"";
		password = L"";
	}
	credentialsView.SetUsername(username);
	credentialsView.SetPassword(password);
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
			try
			{
				credentialsModel.SetCredentials(username, password);
				credentialsView.Close();
			}
			catch (const std::exception & e)
			{
				credentialsView.SetMessage(ConvertToUnicode(e.what()));
			}
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
