#pragma once

#include "INoteListView.h"

class MockNoteListView : public INoteListView
{
public:

	std::vector<std::wstring> notes;
	bool                      updated;

public:

	MockNoteListView();

	virtual void ClearNotes();

	virtual void AddNote(std::wstring note);

	virtual void Update();
};
