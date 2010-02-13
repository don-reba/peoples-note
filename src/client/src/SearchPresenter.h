#pragma once

#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

class SearchPresenter
{
private:

	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	IUserModel     & userModel;

public:

	SearchPresenter
		( INoteListModel & noteListModel
		, IUserModel     & userModel
		, INoteListView  & noteListView
		);

private:

	void OnSearch();

	void ResetNotes();

	void SearchNotes(std::wstring search);
};
