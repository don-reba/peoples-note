#pragma once

#include "ICredentialsModel.h"
#include "ICredentialsView.h"

class CredentialsPresenter
{
private:

	ICredentialsModel & credentialsModel;
	ICredentialsView  & credentialsView;

public:
	
	CredentialsPresenter
		( ICredentialsModel & credentialsModel
		, ICredentialsView  & credentialsView
		);

private:

	void OnUserChanged();
	void OnPasswordChanged();
	void OnSignIn();
};
