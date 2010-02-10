#include "stdafx.h"
#include "NoteListModel.h"

using namespace std;

void NoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}

vector<INote*> NoteListModel::GetNotes()
{
	return notes;
}

void NoteListModel::SetNotes(vector<INote*> notes)
{
	this->notes = notes;
	SignalChanged();
}
