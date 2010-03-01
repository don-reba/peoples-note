#pragma once

#include "INoteListModel.h"
#include "INote.h"

class NoteListModel : public INoteListModel
{
private:

	boost::ptr_vector<INote> notes;

	signal SignalChanged;

public:

	virtual void ConnectChanged(slot_type OnReset);

	virtual boost::ptr_vector<INote> & GetNotes(); 

	virtual void SetNotes(boost::ptr_vector<INote> & notes);
};
