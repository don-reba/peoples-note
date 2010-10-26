#pragma once

#include "DbLocation.h"

class IProfileView;
class INoteListView;
class IUserModel;

class ProfilePresenter
{
private:

	IProfileView  & profileView;
	INoteListView & noteListView;
	IUserModel    & userModel;

public:

	ProfilePresenter
	( IProfileView  & profileView
	, INoteListView & noteListView
	, IUserModel    & userModel
	);

// event handlers

private:

	void OnClose();
	void OnDbMove();
	void OnLoaded();
	void OnProfile();

// utility functions

private:

	void SetDbPath(const std::wstring & path);
	void SetDbSize(__int64 size);
	void SetMoveButtonText(DbLocation location);
	void SetUsername(const std::wstring & username);
};
