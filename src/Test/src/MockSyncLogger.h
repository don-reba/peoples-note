#pragma once

#include "ISyncLogger.h"

class MockSyncLogger : public ISyncLogger
{
public:

	virtual void ListLocalNotes      (const EnInteropNoteList & notes);
	virtual void ListRemoteNotes     (const EnInteropNoteList & notes);
	virtual void ListLocalNotebooks  (const NotebookList      & notebooks);
	virtual void ListRemoteNotebooks (const NotebookList      & notebooks);
	virtual void ListLocalTags       (const TagList           & tags);
	virtual void ListRemoteTags      (const TagList           & tags);

	virtual void BeginSyncStage(const std::wstring & name);

	virtual void Add    (const Guid & remote);
	virtual void Delete (const Guid & local);
	virtual void Rename (const Guid & local);
	virtual void Upload (const Guid & local);
	virtual void Merge  (const Guid & local, const Guid & remote);

	virtual void Error(const std::wstring & message);
};