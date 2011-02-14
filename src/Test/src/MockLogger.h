#pragma once

#include "ILogger.h"

class MockLogger : public ILogger
{
public:

	virtual void Clear();

	virtual void Flush();

	virtual ExceptionMessage GetExceptionMessage();

	virtual void ListNotes     (const std::wstring & listTitle, const EnInteropNoteList & notes);
	virtual void ListNotebooks (const std::wstring & listTitle, const NotebookList      & notebooks);
	virtual void ListTags      (const std::wstring & listTitle, const TagList           & tags);
	virtual void ListGuids     (const std::wstring & listTitle, const std::vector<Guid> & guids);

	virtual void BeginSyncStage(const std::wstring & name);

	virtual void PerformAction
		( const wchar_t * action
		, const Guid    * local
		, const Guid    * remote
		);

	virtual void AuthorizationError
		( const std::wstring & username
		, const std::wstring & message
		);

	virtual void SyncError(const std::wstring & message);
};