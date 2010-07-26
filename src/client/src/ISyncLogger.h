#pragma once

#include "EnInteropNote.h"
#include "Notebook.h"
#include "Tag.h"

class ISyncLogger
{
public:

	virtual void ListNotes     (const std::wstring & listTitle, const EnInteropNoteList & notes)     = 0;
	virtual void ListNotebooks (const std::wstring & listTitle, const NotebookList      & notebooks) = 0;
	virtual void ListTags      (const std::wstring & listTitle, const TagList           & tags)      = 0;
	virtual void ListGuids     (const std::wstring & listTitle, const std::vector<Guid> & guids)     = 0;

	virtual void BeginSyncStage(const std::wstring & name) = 0;

	virtual void PerformAction
		( const wchar_t * action
		, const Guid    * local
		, const Guid    * remote
		) = 0;

	virtual void Error(const std::wstring & message) = 0;
};
