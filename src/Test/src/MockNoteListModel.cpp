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

ptr_vector<INote> & MockNoteListModel::GetNotes()
{
	return notes;
}

void MockNoteListModel::SetNotes(ptr_vector<INote> & notes)
{
	this->notes = notes.release();
}
