#include "stdafx.h"

#include "CredentialsPresenter.h"

using namespace boost;

CredentialsPresenter::CredentialsPresenter
	( ICredentialsModel & credentialsModel
	, ICredentialsView  & credentialsView
	)
	: credentialsModel (credentialsModel)
	, credentialsView  (credentialsView)
{
	credentialsView.ConnectSignIn(bind(&CredentialsPresenter::OnSignIn, this));
	credentialsView.SetUsername(credentialsModel.GetUsername());
	credentialsView.SetPassword(credentialsModel.GetPassword());
}

void CredentialsPresenter::OnSignIn()
{
	credentialsModel.SetCredentials
		( credentialsView.GetUsername()
		, credentialsView.GetPassword()
		);
}
