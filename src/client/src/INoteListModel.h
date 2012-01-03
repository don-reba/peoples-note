#pragma once

#include "ISignalProvider.h"
#include "Note.h"

class INoteListModel : public ISignalProvider
{
	MacroIEvent(Changed)

public:

	virtual ~INoteListModel() {}

	virtual void GetCurrentPage
		( NoteList & notes
		, bool     & hasPreviousPage
		, bool     & hasNextPage
		) = 0;

	virtual bool GetNotebookTitleState() = 0;

	virtual void Reload() = 0;

	virtual void SelectNextPage() = 0;

	virtual void SelectPreviousPage() = 0;

	virtual void SetNotebookTitleState(bool isEnabled) = 0;

	virtual void SetQuery(const std::wstring & query) = 0;
};
