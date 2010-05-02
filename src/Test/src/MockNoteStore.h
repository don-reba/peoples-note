#pragma once
#include "INoteStore.h"

class MockNoteStore : public INoteStore
{
public:

	NoteList     remoteNotes;
	NotebookList remoteNotebooks;
	TagList      remoteTags;

public:

	virtual void DownloadNoteResources();

	virtual void ListEntries
		( NoteList     & notes
		, NotebookList & notebooks
		, TagList      & tags
		);

	virtual void UploadNote(const Note & note);

	virtual void UploadNotebook(const Notebook & notebook);

	virtual void UploadTag(const Tag & tag);
};
