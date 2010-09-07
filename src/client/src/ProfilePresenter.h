#pragma once

class IProfileView;
class INoteListView;

class ProfilePresenter
{
private:

	IProfileView  & profileView;
	INoteListView & noteListView;
public:

	ProfilePresenter
	( IProfileView    & profileView
	, INoteListView & noteListView
	);

private:

	void OnProfile();
	void OnClose();
};
