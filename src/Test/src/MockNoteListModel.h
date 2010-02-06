#pragma once

#include "INoteListModel.h"
#include "MockNote.h"

class MockNoteListModel : public INoteListModel
{
private:

	signal SignalReset;

public:

	std::vector<INote*> notes;

	void Reset();

	virtual void ConnectChanged(slot_type OnReset);

	virtual std::vector<INote*> GetNotes();

	virtual void SetNotes(std::vector<INote*> notes);
};
