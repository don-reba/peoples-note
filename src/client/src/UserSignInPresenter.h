#pragma once

class INoteListView;
class IUserModel;

class UserSignInPresenter
{
private:

	INoteListView & noteListView;
	IUserModel    & userModel;

public:

	UserSignInPresenter
		( INoteListView & noteListView
		, IUserModel    & userModel
		);

private:

	void OnCredentialsUpdated();
	void OnSignIn();
};
