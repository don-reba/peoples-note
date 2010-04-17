#include "stdafx.h"
#include "SearchPresenter.h"

#include "Notebook.h"
#include "Transaction.h"

using namespace boost;
using namespace std;

SearchPresenter::SearchPresenter
	( INoteListModel & noteListModel
	, IUserModel     & userModel
	, INoteListView  & noteListView
	)
	: noteListModel (noteListModel)
	, userModel     (userModel)
	, noteListView  (noteListView)
{
	noteListView.ConnectSearch(bind(&SearchPresenter::OnSearch, *this));
}

void SearchPresenter::OnSearch()
{
	wstring search = noteListView.GetSearchString();
	if (search.find_first_not_of(L" \t") == wstring::npos)
		ResetNotes();
	else
		SearchNotes(search);
}

void SearchPresenter::ResetNotes()
{
	Transaction transaction(userModel);
	noteListModel.SetNotes(userModel.GetNotesByNotebook(userModel.GetLastUsedNotebook()));
}

void SearchPresenter::SearchNotes(wstring search)
{
	Transaction transaction(userModel);
	noteListModel.SetNotes(userModel.GetNotesBySearch(search));
}
