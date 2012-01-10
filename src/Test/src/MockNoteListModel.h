#pragma once
#include "INoteListModel.h"

#include "Note.h"

class MockNoteListModel : public INoteListModel
{
	MacroTestEvent(NoteChanged)
	MacroTestEvent(NoteListChanged)
public:

	NoteList          notes;
	size_t            pageSize;
	std::wstring      query;
	NotebookViewStyle viewStyle;

	bool gotNotifiedOfNoteChange;
	bool hasNextPage;
	bool hasPreviousPage;
	bool isReloaded;
	bool notebookTitleState;
	bool nextPageSelected;
	bool previousPageSelected;

public:

	MockNoteListModel();

	virtual void GetCurrentPage
		( NoteList & notes
		, bool & hasPreviousPage
		, bool & hasNextPage
		);

	virtual bool GetNotebookTitleState();

	virtual NotebookViewStyle GetViewStyle();

	virtual void NotifyOfNoteChange();

	virtual void Reload();

	virtual void SelectNextPage();

	virtual void SelectPreviousPage();

	virtual void SetNotebookTitleState(bool isEnabled);

	virtual void SetPageSize(size_t pageSize);

	virtual void SetQuery(const std::wstring & query);

	virtual void SetViewStyle(NotebookViewStyle style);
};
