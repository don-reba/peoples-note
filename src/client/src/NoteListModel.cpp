#include "stdafx.h"
#include "NoteListModel.h"

#include "INote.h"

using namespace boost;
using namespace std;

void NoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}

ptr_vector<INote> & NoteListModel::GetNotes()
{
	return notes;
}

void NoteListModel::SetNotes(ptr_vector<INote> & notes)
{
	this->notes = notes.release();
	SignalChanged();
}
