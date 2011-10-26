#include "stdafx.h"

#include "CredentialsPresenter.h"
#include "ICredentialsModel.h"
#include "ICredentialsView.h"
#include "IUserStore.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

const wstring anonymousUser(L"[anonymous]");

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
	credentialsModel.Set(anonymousUser, L"");
}

void CredentialsPresenter::OnCommit()
{
	credentialsView.Close();
}

void CredentialsPresenter::OnUpdate()
{
	const wstring username(CreateDisplayUsername(credentialsModel.GetUsername()));

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
	credentialsView.SetUsername(CreateDisplayUsername(credentialsModel.GetUsername()));
	credentialsView.SetPassword(L"");
}

wstring CredentialsPresenter::CreateDisplayUsername(const wstring & username)
{
	if (username == anonymousUser)
		return L"";
	else
		return username;
}
