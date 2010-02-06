#pragma once

#include "INote.h"
#include "ISignalProvider.h"

class INoteListModel : public ISignalProvider
{
public:

	virtual void ConnectChanged(slot_type OnReset) = 0;

	virtual std::vector<INote*> GetNotes() = 0;

	virtual void SetNotes(std::vector<INote*> notes) = 0;
};
