#pragma once

#include "INoteListModel.h"

class NoteListModel : public INoteListModel
{
private:

	std::vector<INote*> notes;

	signal SignalChanged;

public:

	virtual void ConnectChanged(slot_type OnReset);

	virtual std::vector<INote*> GetNotes();

	virtual void SetNotes(std::vector<INote*> notes);
};
