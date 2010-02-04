#include "stdafx.h"

#include "CredentialsPresenter.h"

using namespace boost;

CredentialsPresenter::CredentialsPresenter
	( ICredentialsModel & model
	, ICredentialsView  & view
	)
	: model (model)
	, view  (view)
{
	view.ConnectSignIn(bind(&CredentialsPresenter::OnSignIn, this));
	view.SetUsername(model.GetUsername());
	view.SetPassword(model.GetPassword());
}

void CredentialsPresenter::OnSignIn()
{
	model.SetCredentials
		( view.GetUsername()
		, view.GetPassword()
		);
}