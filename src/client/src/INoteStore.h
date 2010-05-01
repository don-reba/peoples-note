#pragma once

#include "Note.h"
#include "Notebook.h"
#include "Tag.h"

class INoteStore
{
public:

	virtual void DownloadNoteResources
		( NoteList     & notes
		, NotebookList & notebooks
		, TagList      & tags
		) = 0;

	virtual void ListEntries() = 0;

	virtual void UploadNote() = 0;

	virtual void UploadNotebook() = 0;

	virtual void UploadTag() = 0;
};
