#include "stdafx.h"
#include "MockNoteListModel.h"

using namespace std;

void MockNoteListModel::ConnectReset(slot_type OnReset)
{
	SignalReset.connect(OnReset);
}

vector<const INote*> MockNoteListModel::GetNotes() const
{
	vector<const INote*> notes;
	notes.reserve(this->notes.size());
	foreach (const MockNote & note, this->notes)
		notes.push_back(&note);
	return notes;
}

void MockNoteListModel::Reset()
{
	SignalReset();
}
