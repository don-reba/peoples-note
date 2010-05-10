#pragma once

#include "EnInteropNote.h"
#include "Notebook.h"
#include "Tag.h"

class ISyncLogger
{
public:

	virtual void ListLocalNotes      (const EnInteropNoteList & notes)     = 0;
	virtual void ListRemoteNotes     (const EnInteropNoteList & notes)     = 0;
	virtual void ListLocalNotebooks  (const NotebookList      & notebooks) = 0;
	virtual void ListRemoteNotebooks (const NotebookList      & notebooks) = 0;
	virtual void ListLocalTags       (const TagList           & tags)      = 0;
	virtual void ListRemoteTags      (const TagList           & tags)      = 0;

	virtual void BeginSyncStage(const std::wstring & name) = 0;

	virtual void Add    (const Guid & remote) = 0;
	virtual void Delete (const Guid & local) = 0;
	virtual void Rename (const Guid & local) = 0;
	virtual void Upload (const Guid & local) = 0;
	virtual void Merge  (const Guid & local, const Guid & remote) = 0;

	virtual void Error(const std::wstring & message) = 0;
};
