#pragma once

#include "INote.h"
#include "ISignalProvider.h"

class INoteListModel : public ISignalProvider
{
public:

	virtual void ConnectReset(slot_type OnReset) = 0;

	virtual std::vector<const INote*> GetNotes() const = 0;
};
