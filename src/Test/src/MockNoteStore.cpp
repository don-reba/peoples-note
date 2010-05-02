#include "stdafx.h"
#include "MockNoteStore.h"

void MockNoteStore::DownloadNoteResources()
{
}

void MockNoteStore::ListEntries
	( NoteList     & notes
	, NotebookList & notebooks
	, TagList      & tags
	)
{
	notes     = remoteNotes;
	notebooks = remoteNotebooks;
	tags      = remoteTags;
}

void MockNoteStore::UploadNote(const Note & note)
{
}

void MockNoteStore::UploadNotebook(const Notebook & notebook)
{
}

void MockNoteStore::UploadTag(const Tag & tag)
{
}
