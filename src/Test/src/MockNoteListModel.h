#pragma once

#include "INoteListModel.h"
#include "MockNote.h"

class MockNoteListModel : public INoteListModel
{
private:

	signal SignalReset;

public:

	std::vector<MockNote> notes;

public:

	virtual void ConnectReset(slot_type OnReset);

	virtual std::vector<const INote*> GetNotes() const;

	void Reset();
};
