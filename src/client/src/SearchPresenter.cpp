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
	noteListModel.SetQuery(L"");
	noteListView.SetSearchButtonToSearch();
	noteListModel.Reload();
}

void SearchPresenter::OnSearch()
{
	wstring search = noteListView.GetSearchText();
	if (search.find_first_not_of(L" \t") == wstring::npos)
		noteListView.SetSearchButtonToSearch();
	else
		noteListView.SetSearchButtonToClear();
	noteListModel.SetQuery(search);
	noteListModel.Reload();
}

void SearchPresenter::OnSearchChanged()
{
	noteListView.SetSearchButtonToSearch();
}
