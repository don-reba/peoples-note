#include "stdafx.h"

#include "CredentialsPresenter.h"
#include "ICredentialsModel.h"
#include "ICredentialsView.h"
#include "IUserStore.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

CredentialsPresenter::CredentialsPresenter
	( ICredentialsModel & credentialsModel
	, ICredentialsView  & credentialsView
	)
	: credentialsModel (credentialsModel)
	, credentialsView  (credentialsView)
{
	credentialsModel.ConnectCommit (bind(&CredentialsPresenter::OnCommit,      this));
	credentialsModel.ConnectUpdate (bind(&CredentialsPresenter::OnUpdate,      this));
	credentialsView.ConnectCreated (bind(&CredentialsPresenter::OnViewCreated, this));
	credentialsView.ConnectOk      (bind(&CredentialsPresenter::OnOk,          this));
	credentialsView.ConnectCancel  (bind(&CredentialsPresenter::OnCancel,      this));
}

void CredentialsPresenter::OnOk()
{
	credentialsModel.Set
		( credentialsView.GetUsername()
		, credentialsView.GetPassword()
		);
}

void CredentialsPresenter::OnCancel()
{
	credentialsModel.Set(L"[anonymous]", L"");
}

void CredentialsPresenter::OnCommit()
{
	credentialsView.Close();
}

void CredentialsPresenter::OnUpdate()
{
	const wstring username(credentialsModel.GetUsername());

	credentialsView.Open();
	credentialsView.SetUsername(username);
	credentialsView.SetPassword(L"");
	credentialsView.SetMessage(credentialsModel.GetStatus());

	if (username.empty())
		credentialsView.SetFocusToUsername();
	else
		credentialsView.SetFocusToPassword();
}

void CredentialsPresenter::OnViewCreated()
{
	wstring username(credentialsModel.GetUsername());
	if (username == L"[anonymous]")
		username = L"";
	credentialsView.SetUsername(username);
	credentialsView.SetPassword(L"");
}
