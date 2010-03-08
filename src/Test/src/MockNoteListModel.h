#pragma once

#include "INoteListModel.h"
#include "Note.h"

class MockNoteListModel : public INoteListModel
{
private:

	signal SignalReset;

public:

	NoteList notes;

	void Reset();

	virtual void ConnectChanged(slot_type OnReset);

	virtual const NoteList & GetNotes();

	virtual void SetNotes(const NoteList & notes);
};
