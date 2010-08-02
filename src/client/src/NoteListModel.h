#pragma once

#include "INoteListModel.h"
#include "Note.h"

class NoteListModel : public INoteListModel
{
private:

	const int pageSize;

	NoteList notes;

	int currentPage;

	signal SignalChanged;

public:

	NoteListModel(int pageSize);

	virtual void ConnectChanged(slot_type OnReset);

	virtual void GetCurrentPage
		( NoteList::const_iterator & begin
		, NoteList::const_iterator & end
		);

	virtual bool HasNextNotes();

	virtual bool HasPreviousNotes();

	virtual void SelectNextPage();

	virtual void SelectPreviousPage();

	virtual void SetNotes(const NoteList & notes);
};
