#pragma once

#include "INoteListView.h"

class MockNoteListView : public INoteListView
{
public:

	std::vector<std::wstring> notes;
	std::vector<std::wstring> notebooks;
	bool                      notesUpdated;
	bool                      notebooksUpdated;

	signal SignalCreated;

public:

	MockNoteListView();

	void Created();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ClearNotes();

	virtual void AddNote(std::wstring noteHtml);

	virtual void UpdateNotes();

	virtual void ClearNotebooks();

	virtual void AddNotebook(std::wstring notebook);

	virtual void UpdateNotebooks();
};
