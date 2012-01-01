#include "stdafx.h"
#include "NoteListModel.h"
#include <vector>

#include "IRegistryKey.h"
#include "IUserModel.h"
#include "Transaction.h"

using namespace boost;
using namespace std;

NoteListModel::NoteListModel
	( int            pageSize
	, IUserModel   & userModel
	, IRegistryKey & registryKey
	)
	: currentPage  (0)
	, hasNextPage (false)
	, pageSize     (pageSize)
	, registryKey  (registryKey)
	, userModel    (userModel)
{
}

void NoteListModel::GetCurrentPage(NoteList & notes)
{
	Transaction transaction(userModel);
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	// ask for one more note than necessary to tell whether there is another page
	userModel.GetNotesBySearch(notebook.guid, query, currentPage * pageSize, pageSize + 1, notes);
	hasNextPage = notes.size() > pageSize;
	if (hasNextPage)
		notes.pop_back();
}

bool NoteListModel::GetNotebookTitleState()
{
	wstring state = registryKey.GetString(L"notebook title state", L"disabled");
	return state == L"enabled";
}

bool NoteListModel::HasNextPage()
{
	return hasNextPage;
}

bool NoteListModel::HasPreviousPage()
{
	return currentPage > 0;
}

void NoteListModel::Reload()
{
	currentPage = 0;
	SignalChanged();
}

void NoteListModel::SelectNextPage()
{
	if (!HasNextPage())
		throw std::exception("Invalid call to NoteListModel::SelectNextPage.");
	++currentPage;
	SignalChanged();
}

void NoteListModel::SelectPreviousPage()
{
	if (!HasPreviousPage())
		throw std::exception("Invalid call to NoteListModel::SelectPreviousPage.");
	--currentPage;
	SignalChanged();
}

void NoteListModel::SetNotebookTitleState(bool isEnabled)
{
	registryKey.SetString
		( L"notebook title state"
		, isEnabled ? L"enabled" : L"disabled"
		);
}

void NoteListModel::SetQuery(const wstring & query)
{
	this->query = query;
}
