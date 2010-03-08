#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace boost;
using namespace std;

void MockNoteListModel::Reset()
{
	SignalReset();
}

void MockNoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalReset.connect(OnReset);
}

const NoteList & MockNoteListModel::GetNotes()
{
	return notes;
}

void MockNoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = notes;
}
