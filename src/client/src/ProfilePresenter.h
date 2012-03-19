#pragma once

#include "DbLocation.h"

class IFlashCard;
class IProfileView;
class INoteListView;
class IUserModel;

class ProfilePresenter
{
private:

	IFlashCard    & flashCard;
	IProfileView  & profileView;
	INoteListView & noteListView;
	IUserModel    & userModel;

public:

	ProfilePresenter
		( IFlashCard    & flashCard
		, IProfileView  & profileView
		, INoteListView & noteListView
		, IUserModel    & userModel
		);

// event handlers

private:

	void OnClose();
	void OnCompact();
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
