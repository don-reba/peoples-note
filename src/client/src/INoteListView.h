#pragma once

#include "ISignalProvider.h"

class INoteListView : public ISignalProvider
{
public:

	virtual void ConnectCreated(slot_type OnCreated) = 0;

	virtual void ClearNotes() = 0;

	virtual void AddNote(std::wstring note) = 0;

	virtual void UpdateNotes() = 0;

	virtual void ClearNotebooks() = 0;

	virtual void AddNotebook(std::wstring notebook) = 0;

	virtual void UpdateNotebooks() = 0;
};
