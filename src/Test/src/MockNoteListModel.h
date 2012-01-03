#pragma once

#include "INoteListModel.h"
#include "Note.h"

class MockNoteListModel : public INoteListModel
{
public:

	NoteList     notes;
	std::wstring query;

	bool hasNextPage;
	bool hasPreviousPage;
	bool isReloaded;
	bool notebookTitleState;
	bool nextPageSelected;
	bool previousPageSelected;

	signal SignalChanged;

public:

	MockNoteListModel();

	virtual void ConnectChanged(slot_type OnReset);

	virtual void GetCurrentPage
		( NoteList & notes
		, bool & hasPreviousPage
		, bool & hasNextPage
		);

	virtual bool GetNotebookTitleState();

	virtual void Reload();

	virtual void SelectNextPage();

	virtual void SelectPreviousPage();

	virtual void SetNotebookTitleState(bool isEnabled);

	virtual void SetQuery(const std::wstring & query);
};
