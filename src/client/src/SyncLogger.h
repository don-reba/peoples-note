#pragma once

#include "ISyncLogger.h"

#include <fstream>

class SyncLogger : public ISyncLogger
{
private:

	std::wofstream stream;

public:

	SyncLogger(const std::wstring & documentPath);

// ISyncLogger implementation

public:

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

	virtual void Error(const std::wstring & message);

// utility functions

private:

	void WriteListHeader(const std::wstring & text);

	void WriteListEntry
		( const std::wstring & name
		, const Guid & guid
		);

	void WriteOperation
		( const wchar_t * name
		, const Guid    & guid
		);

	void WriteOperation
		( const wchar_t * name
		, const Guid    & guid1
		, const Guid    & guid2
		);
};
