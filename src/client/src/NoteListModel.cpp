#include "stdafx.h"
#include "NoteListModel.h"

#include "Note.h"

using namespace boost;
using namespace std;

NoteListModel::NoteListModel()
	: notes (&empty)
{
}

void NoteListModel::ConnectChanged(slot_type OnReset)
{
	SignalChanged.connect(OnReset);
}

const NoteList & NoteListModel::GetNotes()
{
	return *notes;
}

void NoteListModel::SetNotes(const NoteList & notes)
{
	this->notes = &notes;
	SignalChanged();
}
