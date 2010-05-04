#include "stdafx.h"
#include "MockNoteStore.h"

void MockNoteStore::GetNoteBody
	( const Note   & note
	, std::wstring & content
	)
{
	Guid guid(note.GetGuid());
	NoteBodyMap::iterator i(noteBodies.find(guid));
	if (i != noteBodies.end())
		content = i->second;
}

void MockNoteStore::GetNoteResource
	( const Guid & guid
	, Resource   & resource
	)
{
	foreach (Resource & r, this->resources)
	{
		if (r.Guid == guid)
		{
			resource = r;
			return;
		}
	}
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


