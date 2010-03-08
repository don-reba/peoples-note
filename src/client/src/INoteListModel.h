#pragma once

#include "ISignalProvider.h"
#include "Note.h"

class INoteListModel : public ISignalProvider
{
public:

	virtual ~INoteListModel() {}

	virtual void ConnectChanged(slot_type OnReset) = 0;

	virtual const NoteList & GetNotes() = 0;

	virtual void SetNotes(const NoteList & notes) = 0;
};
