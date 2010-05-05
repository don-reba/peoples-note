#include "stdafx.h"
#include "MockNoteStore.h"

#include <algorithm>

using namespace std;

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
	createdNotes.push_back(note);
	copy
		( resources.begin()
		, resources.end()
		, back_inserter(createdResources)
		);
}

void MockNoteStore::CreateNotebook
	( const Notebook & notebook
	)
{
	createdNotebooks.push_back(notebook);
}

void MockNoteStore::CreateTag
	( const Tag & tag
	)
{
	createdTags.push_back(tag);
}


