#pragma once

class ICredentialsModel;
class IEnService;
class INoteListView;
class IUserModel;

class UserSignInPresenter
{
private:

	ICredentialsModel & credentialsModel;
	IEnService        & enService;
	INoteListView     & noteListView;
	IUserModel        & userModel;

public:

	UserSignInPresenter
		( ICredentialsModel & credentialsModel
		, IEnService        & enService
		, INoteListView     & noteListView
		, IUserModel        & userModel
		);

private:

	void OnCredentialsSet();
	void OnSignIn();
};
