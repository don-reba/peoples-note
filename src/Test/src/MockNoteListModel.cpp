#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace boost;
using namespace std;

MockNoteListModel::MockNoteListModel()
	: hasNextNotes         (false)
	, hasPreviousNotes     (false)
	, isReloaded           (false)
	, nextPageSelected     (false)
	, previousPageSelected (false)
{
}

void MockNoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}


void MockNoteListModel::GetCurrentPage
	( NoteList::const_iterator & begin
	, NoteList::const_iterator & end
	)
{
	begin = notes.begin();
	end   = notes.end();
}

bool MockNoteListModel::HasNextNotes()
{
	return hasNextNotes;
}

bool MockNoteListModel::HasPreviousNotes()
{
	return hasPreviousNotes;
}

void MockNoteListModel::Reload()
{
	isReloaded = true;
}

void MockNoteListModel::SelectNextPage()
{
	nextPageSelected = true;
}

void MockNoteListModel::SelectPreviousPage()
{
	previousPageSelected = true;
}


void MockNoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = notes;
}
