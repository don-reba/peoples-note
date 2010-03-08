#pragma once

#include "INoteListModel.h"
#include "Note.h"

class NoteListModel : public INoteListModel
{
private:

	const NoteList * notes;
	const NoteList   empty;

	signal SignalChanged;

public:

	NoteListModel();

	virtual void ConnectChanged(slot_type OnReset);

	virtual const NoteList & GetNotes(); 

	virtual void SetNotes(const NoteList & notes);
};
