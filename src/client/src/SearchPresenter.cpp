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
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	noteListModel.SetNotes(notes);
}

void SearchPresenter::SearchNotes(wstring search)
{
	Transaction transaction(userModel);
	NoteList notes;
	userModel.GetNotesBySearch(search, notes);
	noteListModel.SetNotes(notes);
}
