#pragma once

#include "ISignalProvider.h"

class INote;

class INoteListModel : public ISignalProvider
{
public:

	virtual ~INoteListModel() {}

	virtual void ConnectChanged(slot_type OnReset) = 0;

	virtual boost::ptr_vector<INote> & GetNotes() = 0;

	virtual void SetNotes(boost::ptr_vector<INote> & notes) = 0;
};
