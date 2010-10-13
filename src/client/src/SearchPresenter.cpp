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
	noteListView.ConnectClearSearch
		(bind(&SearchPresenter::OnClearSearch, *this));

	noteListView.ConnectSearch
		(bind(&SearchPresenter::OnSearch, *this));

	noteListView.ConnectSearchChanged
		(bind(&SearchPresenter::OnSearchChanged, *this));
}

void SearchPresenter::OnClearSearch()
{
	noteListView.SetSearchText(L"");
	ResetNotes();
}

void SearchPresenter::OnSearch()
{
	wstring search = noteListView.GetSearchString();
	if (search.find_first_not_of(L" \t") == wstring::npos)
		ResetNotes();
	else
		SearchNotes(search);
}

void SearchPresenter::OnSearchChanged()
{
	noteListView.SetSearchButtonToSearch();
}

void SearchPresenter::ResetNotes()
{
	noteListView.SetSearchButtonToSearch();
	Transaction transaction(userModel);
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	noteListModel.Reload();
}

void SearchPresenter::SearchNotes(wstring search)
{
	noteListView.SetSearchButtonToClear();

	Transaction transaction(userModel);
	NoteList notes;
	userModel.GetNotesBySearch(search, notes);
	noteListModel.SetNotes(notes);
}
