#pragma once
#include "INoteStore.h"

class MockNoteStore : public INoteStore
{
public:

	virtual void DownloadNoteResources
		( NoteList     & notes
		, NotebookList & notebooks
		, TagList      & tags
		);

	virtual void ListEntries();

	virtual void UploadNote();

	virtual void UploadNotebook();

	virtual void UploadTag();
};
