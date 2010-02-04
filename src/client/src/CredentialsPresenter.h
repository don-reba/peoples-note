#pragma once

#include "ICredentialsModel.h"
#include "ICredentialsView.h"

class CredentialsPresenter
{
private:

	ICredentialsModel & model;
	ICredentialsView  & view;

public:
	
	CredentialsPresenter
		( ICredentialsModel & model
		, ICredentialsView  & view
		);

private:

	void OnUserChanged();
	void OnPasswordChanged();
	void OnSignIn();
};
