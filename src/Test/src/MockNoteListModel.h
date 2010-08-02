#pragma once

#include "INoteListModel.h"
#include "Note.h"

class MockNoteListModel : public INoteListModel
{
public:

	NoteList notes;

	bool hasNextNotes;
	bool hasPreviousNotes;

	bool nextPageSelected;
	bool previousPageSelected;

	signal SignalChanged;

public:

	MockNoteListModel();

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
