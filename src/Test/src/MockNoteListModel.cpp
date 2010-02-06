#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace std;

void MockNoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalReset.connect(OnReset);
}

vector<INote*> MockNoteListModel::GetNotes()
{
	return notes;
}

void MockNoteListModel::Reset()
{
	SignalReset();
}

void MockNoteListModel::SetNotes(vector<INote*> notes)
{
	this->notes = notes;
}
