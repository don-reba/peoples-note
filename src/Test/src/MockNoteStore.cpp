#include "stdafx.h"
#include "MockNoteStore.h"

#include <algorithm>

using namespace std;

void MockNoteStore::CreateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, const Guid             & notebook
	, Note                   & replacement
	)
{
	createdNotes.push_back(note);
	noteBodies[note.guid] = body;
	copy
		( resources.begin()
		, resources.end()
		, back_inserter(createdResources)
		);
	replacement = replacementNote;
}

void MockNoteStore::CreateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	createdNotebooks.push_back(notebook);
	replacement = replacementNotebook;
}

void MockNoteStore::CreateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	createdTags.push_back(tag);
	replacement = replacementTag;
}

void MockNoteStore::GetDefaultNotebook(Guid & notebook)
{
	notebook = defaultNotebook;
}

void MockNoteStore::GetNoteBody
	( const Note   & note
	, std::wstring & content
	)
{
	NoteBodyMap::iterator i(noteBodies.find(note.guid));
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

void MockNoteStore::GetNoteTagNames
	( const Note                & note
	, std::vector<std::wstring> & names
	)
{
	typedef pair<string, wstring> Pair;
	foreach (Pair & p, noteTags)
	{
		if (p.first == static_cast<string>(note.guid))
			names.push_back(p.second);
	}
}

void MockNoteStore::GetSyncState(SyncState & syncState)
{
	syncState = this->syncState;
}

void MockNoteStore::ListEntries
	( EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	, const Guid        & notebookFilter
	)
{
	this->notebookFilter = notebookFilter;
	foreach (EnInteropNote & note, remoteNotes)
	{
		if (note.notebook == notebookFilter)
			notes.push_back(note);
	}
	copy(remoteNotebooks.begin(), remoteNotebooks.end(), back_inserter(notebooks));
	copy(remoteTags.begin(),      remoteTags.end(),      back_inserter(tags));
}

void MockNoteStore::ListEntries
	( int                 globalUpdateCount
	, int                 notebookUpdateCount
	, EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	, vector<Guid>      & expungedNotes
	, vector<Guid>      & expungedNotebooks
	, vector<Guid>      & expungedTags
	, vector<Guid>      & resources
	, const Guid        & notebookFilter
	)

{
	ListEntries(notes, notebooks, tags, notebookFilter);
	copy
		( this->expungedNotes.begin()
		, this->expungedNotes.end()
		, back_inserter(expungedNotes)
		);
	copy
		( this->expungedNotebooks.begin()
		, this->expungedNotebooks.end()
		, back_inserter(expungedNotebooks)
		);
	copy
		( this->expungedTags.begin()
		, this->expungedTags.end()
		, back_inserter(expungedTags)
		);
	foreach (Resource & resource, this->resources)
		resources.push_back(resource.Guid);
}

void MockNoteStore::UpdateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, const Guid             & notebook
	, Note                   & replacement
	)
{
	foreach (Note & n, createdNotes)
	{
		if (n.guid != note.guid)
			continue;
		n = note;
		break;
	}
	noteBodies[note.guid] = body;
	for (int i = 0; i != this->resources.size(); ++i)
	{
		const Resource & r(resources.at(i));
		if (r.Note != note.guid)
			continue;
		this->resources.erase(this->resources.begin() + i);
		--i;
	}
	foreach (const Resource & r, resources)
		std::copy
			( resources.begin()
			, resources.end()
			, back_inserter(this->resources)
			);
	replacement = replacementNote;
}

void MockNoteStore::UpdateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	foreach (Notebook & n, createdNotebooks)
	{
		if (n.guid != notebook.guid)
			continue;
		n = notebook;
		break;
	}
	replacement = replacementNotebook;
}

void MockNoteStore::UpdateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	foreach (Tag & t, createdTags)
	{
		if (t.guid != tag.guid)
			continue;
		t = tag;
		break;
	}
	replacement = replacementTag;
}
