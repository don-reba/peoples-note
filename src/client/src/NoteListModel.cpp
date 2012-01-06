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
	: currentPage (0)
	, pageSize    (pageSize)
	, registryKey (registryKey)
	, userModel   (userModel)
{
}

void NoteListModel::GetCurrentPage
	( NoteList & notes
	, bool     & hasPreviousPage
	, bool     & hasNextPage
	)
{
	Transaction transaction(userModel);
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);

	// ask for one more note than necessary to tell whether there is another page
	userModel.GetNotesBySearch(notebook.guid, query, currentPage * pageSize, pageSize + 1, notes);
	hasPreviousPage = currentPage > 0;
	hasNextPage     = notes.size() > pageSize;
	if (hasNextPage)
		notes.pop_back();
}

bool NoteListModel::GetNotebookTitleState()
{
	wstring state(registryKey.GetString(L"notebook title state", L"disabled"));
	return state == L"enabled";
}

NotebookViewStyle NoteListModel::GetViewStyle()
{
	wstring styleName(registryKey.GetString(L"notebook view style", L"combined"));
	if (styleName == L"titles")
		return NotebookViewTitles;
	return NotebookViewCombined;
}

void NoteListModel::Reload()
{
	currentPage = 0;
	SignalChanged();
}

void NoteListModel::SelectNextPage()
{
	++currentPage;
	SignalChanged();
}

void NoteListModel::SelectPreviousPage()
{
	if (currentPage <= 0)
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

void NoteListModel::SetViewStyle(NotebookViewStyle style)
{
	const wchar_t * styleName(L"combined");
	if (style == NotebookViewTitles)
		styleName = L"titles";
	registryKey.SetString(L"notebook view style", styleName);
}
