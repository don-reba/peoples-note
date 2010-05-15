#pragma once

#include "INoteListModel.h"
#include "Note.h"

class MockNoteListModel : public INoteListModel
{
public:

	signal SignalChanged;

public:

	NoteList notes;

	virtual void ConnectChanged(slot_type OnChanged);

	virtual const NoteList & GetNotes();

	virtual void SetNotes(const NoteList & notes);
};
