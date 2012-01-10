#pragma once
#include "ISignalProvider.h"

#include "Note.h"
#include "NotebookViewStyle.h"

class INoteListModel : public ISignalProvider
{
	MacroIEvent(NoteChanged)
	MacroIEvent(NoteListChanged)

public:

	virtual ~INoteListModel() {}

	virtual void GetCurrentPage
		( NoteList & notes
		, bool     & hasPreviousPage
		, bool     & hasNextPage
		) = 0;

	virtual bool GetNotebookTitleState() = 0;

	virtual NotebookViewStyle GetViewStyle() = 0;

	virtual void NotifyOfNoteChange() = 0;

	virtual void Reload() = 0;

	virtual void SelectNextPage() = 0;

	virtual void SelectPreviousPage() = 0;

	virtual void SetNotebookTitleState(bool isEnabled) = 0;

	virtual void SetPageSize(size_t pageSize) = 0;

	virtual void SetQuery(const std::wstring & query) = 0;

	virtual void SetViewStyle(NotebookViewStyle style) = 0;
};
