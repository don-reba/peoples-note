#pragma once

class IAboutView;
class INoteListView;

class AboutPresenter
{
private:

	IAboutView    & aboutView;
	INoteListView & noteListView;
public:

	AboutPresenter
	( IAboutView    & aboutView
	, INoteListView & noteListView
	);

private:

	void OnAbout();
	void OnClose();
};
