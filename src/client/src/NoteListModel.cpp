#include "stdafx.h"
#include "NoteListModel.h"
#include <vector>

#include "IRegistryKey.h"
#include "IUserModel.h"
#include "Transaction.h"

using namespace boost;
using namespace std;

NoteListModel::NoteListModel
	( IUserModel   & userModel
	, IRegistryKey & registryKey
	)
	: firstNote   (0)
	, pageSize    (0)
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

	Guid notebookGuid;
	userModel.GetLastUsedNotebook(notebookGuid);

	// ask for one more note than necessary to tell whether there is another page
	userModel.GetNotes(notebookGuid, query, firstNote, pageSize + 1, notes);

	hasPreviousPage = firstNote > 0;
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

void NoteListModel::NotifyOfNoteChange()
{
	SignalNoteChanged();
}

void NoteListModel::Reload()
{
	firstNote = 0;
	SignalNoteListChanged();
}

void NoteListModel::SelectNextPage()
{
	firstNote += pageSize;
	SignalNoteListChanged();
}

void NoteListModel::SelectPreviousPage()
{
	firstNote -= pageSize;
	if (firstNote < 0)
		throw std::exception("Invalid call to NoteListModel::SelectPreviousPage.");
	SignalNoteListChanged();
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

void NoteListModel::SetPageSize(size_t pageSize)
{
	this->pageSize = pageSize;
}

void NoteListModel::SetViewStyle(NotebookViewStyle style)
{
	const wchar_t * styleName(L"combined");
	if (style == NotebookViewTitles)
		styleName = L"titles";
	registryKey.SetString(L"notebook view style", styleName);
}
