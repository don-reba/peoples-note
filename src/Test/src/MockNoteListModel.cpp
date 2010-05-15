#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace boost;
using namespace std;

void MockNoteListModel::ConnectChanged(slot_type OnChanged)
{
	SignalChanged.connect(OnChanged);
}

const NoteList & MockNoteListModel::GetNotes()
{
	return notes;
}

void MockNoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = notes;
}
