#include "stdafx.h"
#include "NoteListModel.h"
#include <vector>

#include "Note.h"

using namespace boost;
using namespace std;

NoteListModel::NoteListModel(int pageSize)
	: currentPage (0)
	, pageSize    (pageSize)
{
}

void NoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}


void NoteListModel::GetCurrentPage
	( NoteList::const_iterator & begin
	, NoteList::const_iterator & end
	)
{
	// the checks could be simpler, if not for iterator verification
	begin =
		(notes.end() - notes.begin() > currentPage * pageSize)
		? notes.begin() + currentPage * pageSize
		: notes.end()
		;
	end =
		(notes.end() - begin > pageSize)
		? begin + pageSize
		: notes.end()
		;
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


void NoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = notes;
	currentPage = 0;
	SignalChanged();
}
