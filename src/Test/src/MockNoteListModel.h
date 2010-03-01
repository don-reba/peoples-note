#pragma once

#include "INoteListModel.h"
#include "MockNote.h"

class MockNoteListModel : public INoteListModel
{
private:

	signal SignalReset;

public:

	boost::ptr_vector<INote> notes;

	void Reset();

	virtual void ConnectChanged(slot_type OnReset);

	virtual boost::ptr_vector<INote> & GetNotes();

	virtual void SetNotes(boost::ptr_vector<INote> & notes);
};
