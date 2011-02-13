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
	( NoteList::const_iterator & begin
	, NoteList::const_iterator & end
	)
{
	// the checks could be simpler, if not for iterator verification
	begin = notes.begin() + currentPage * pageSize;
	end =
		(notes.end() - begin > pageSize)
		? begin + pageSize
		: notes.end()
		;
}

bool NoteListModel::GetNotebookTitleState()
{
	wstring state = registryKey.GetString(L"notebook title state", L"disabled");
	return state == L"enabled";
}

bool NoteListModel::HasNextNotes()
{
	int notesUpToCurrent((currentPage + 1) * pageSize);
	return notesUpToCurrent < static_cast<int>(notes.size());
}

bool NoteListModel::HasPreviousNotes()
{
	return currentPage > 0;
}

void NoteListModel::Reload()
{
	Transaction transaction(userModel);
	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.GetNotesByNotebook(notebook, notes);
	currentPage = 0;
	SignalChanged();
}

void NoteListModel::SelectNextPage()
{
	if (!HasNextNotes())
		throw std::exception("Invalid call to NoteListModel::SelectNextPage.");
	++currentPage;
	SignalChanged();
}

void NoteListModel::SelectPreviousPage()
{
	if (!HasPreviousNotes())
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

void NoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = notes;
	currentPage = 0;
	SignalChanged();
}
