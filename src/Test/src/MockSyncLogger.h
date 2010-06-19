#pragma once

#include "ISyncLogger.h"

class MockSyncLogger : public ISyncLogger
{
public:

	virtual void ListNotes     (const std::wstring & listTitle, const EnInteropNoteList & notes);
	virtual void ListNotebooks (const std::wstring & listTitle, const NotebookList      & notebooks);
	virtual void ListTags      (const std::wstring & listTitle, const TagList           & tags);
	virtual void ListGuids     (const std::wstring & listTitle, const std::vector<Guid> & guids);

	virtual void BeginSyncStage(const std::wstring & name);

	virtual void Add    (const Guid & remote);
	virtual void Delete (const Guid & local);
	virtual void Rename (const Guid & local);
	virtual void Upload (const Guid & local);
	virtual void Merge  (const Guid & local, const Guid & remote);

	virtual void Error(const std::wstring & message);
};