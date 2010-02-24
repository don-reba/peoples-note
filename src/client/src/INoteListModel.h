#pragma once

#include "ISignalProvider.h"

class INote;

class INoteListModel : public ISignalProvider
{
public:

	virtual void ConnectChanged(slot_type OnReset) = 0;

	virtual std::vector<INote*> GetNotes() = 0;

	virtual void SetNotes(std::vector<INote*> notes) = 0;
};
