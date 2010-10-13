#pragma once

#include "ISignalProvider.h"
#include "Note.h"

class INoteListModel : public ISignalProvider
{
public:

	virtual ~INoteListModel() {}

	virtual void ConnectChanged(slot_type OnReset) = 0;

	virtual void GetCurrentPage
		( NoteList::const_iterator & begin
		, NoteList::const_iterator & end
		) = 0;

	virtual bool HasNextNotes() = 0;

	virtual bool HasPreviousNotes() = 0;

	virtual void Reload() = 0;

	virtual void SelectNextPage() = 0;

	virtual void SelectPreviousPage() = 0;

	virtual void SetNotes(const NoteList & notes) = 0;
};
