#pragma once

class ICredentialsModel;
class ICredentialsView;

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
