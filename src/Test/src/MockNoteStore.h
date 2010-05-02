#pragma once
#include "INoteStore.h"

class MockNoteStore : public INoteStore
{
public:

	virtual void DownloadNoteResources();

	virtual void ListEntries
		( NoteList     & notes
		, NotebookList & notebooks
		, TagList      & tags
		);

	virtual void UploadNote();

	virtual void UploadNotebook();

	virtual void UploadTag();
};
