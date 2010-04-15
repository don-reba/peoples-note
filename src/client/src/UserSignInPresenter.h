#pragma once

class ICredentialsModel;
class INoteListView;
class IUserModel;

class UserSignInPresenter
{
private:

	ICredentialsModel & newCredentials;
	INoteListView     & noteListView;
	IUserModel        & userModel;

public:

	UserSignInPresenter
		( ICredentialsModel & newCredentials
		, INoteListView     & noteListView
		, IUserModel        & userModel
		);

private:

	void OnCredentialsUpdated();
	void OnSignIn();
};
