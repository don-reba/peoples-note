#include "stdafx.h"
#include "MockNoteStore.h"

void MockNoteStore::GetNoteBody
	( const Note   & Note
	, std::wstring & content
	)
{
}

void MockNoteStore::GetNoteResource
	( const Guid & guid
	, Resource   & resource
	)
{
}

void MockNoteStore::ListEntries
	( EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	)
{
	notes     = remoteNotes;
	notebooks = remoteNotebooks;
	tags      = remoteTags;
}

void MockNoteStore::CreateNote
	( const Note                  & note
	, const std::vector<Resource> & resources
	)
{
}

void MockNoteStore::CreateNotebook
	( const Notebook & notebook
	)
{
}

void MockNoteStore::CreateTag
	( const Tag & tag
	)
{
}


