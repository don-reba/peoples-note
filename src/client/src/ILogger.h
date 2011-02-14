#pragma once

#include "EnInteropNote.h"
#include "ExceptionMessage.h"
#include "Notebook.h"
#include "Tag.h"

class ILogger
{
public:

	virtual void Clear() = 0;

	virtual void Flush() = 0;

	virtual ExceptionMessage GetExceptionMessage() = 0;

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

	virtual void AuthorizationError
		( const std::wstring & username
		, const std::wstring & message
		) = 0;

	virtual void SyncError(const std::wstring & message) = 0;
};
