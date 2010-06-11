#include "stdafx.h"
#include "NoteStore.h"
#include "Tools.h"

#include <algorithm>

using namespace std;
using namespace Tools;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote;

//----------
// interface
//----------

const wchar_t * const NoteStore::baseUrl
	( L"http://sandbox.evernote.com/edam/note/"
	);

NoteStore::NoteStore
	( const AuthenticationToken & token
	, const wstring             & shardId
	)
	: token     (token)
	, transport ((baseUrl + shardId).c_str())
	, protocol  (transport)
	, noteStore (protocol)
{
	transport.Open();
}

//--------------------------
// INoteStore implementation
//--------------------------

void NoteStore::GetNoteBody
	( const Note & note
	, wstring    & content
	)
{
	content = noteStore.getNoteContent
		( token
		, ConvertToUnicode(note.guid)
		);
}

void NoteStore::GetNoteResource
	( const Guid & guid
	, Resource   & resource
	)
{
	EDAM::Types::Resource enResource = noteStore.getResource
		( token                  // authenticationToken
		, ConvertToUnicode(guid) // guid
		, true                   // withData
		, false                  // withRecognition
		, false                  // withAttributes
		, false                  // withAlternateData
		);
	copy
		( enResource.data.body.begin()
		, enResource.data.body.end()
		, back_inserter(resource.Data)
		);
	resource.Hash = HashWithMD5(resource.Data);
	resource.Guid = enResource.guid;
	resource.Note = enResource.noteGuid;
}

void NoteStore::GetNoteTagNames
	( const Note      & note
	, vector<wstring> & names
	)
{
	wstring guid(ConvertToUnicode(note.guid));
	names.swap(noteStore.getNoteTagNames(token, guid));
}

void NoteStore::ListEntries
	( EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	)
{
	EDAM::NoteStore::SyncChunk chunk;
	chunk.chunkHighUSN = 0;
	chunk.updateCount  = 1;
	while (chunk.chunkHighUSN < chunk.updateCount)
	{
		chunk = noteStore.getSyncChunk(token, chunk.chunkHighUSN, 20, true);
		ListEntries(chunk, notes, notebooks, tags);
	}
}

void NoteStore::CreateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, Note                   & replacement
	)
{
	EDAM::Types::Note enNote;
	enNote.__isset.title     = true;
	enNote.__isset.created   = true;
	enNote.__isset.content   = true;
	enNote.__isset.resources = true;

	enNote.title   = note.name;
	enNote.created = note.creationDate.GetTime() * 1000;
	enNote.content = body;

	enNote.resources.resize(resources.size());
	for (int i(0); i != resources.size(); ++i)
	{
		EDAM::Types::Resource & resource(enNote.resources.at(i));
		resource.__isset.data = true;
		resource.data.__isset.body = true;
		resource.data.__isset.size = true;

		copy
			( resources.at(i).Data.begin()
			, resources.at(i).Data.end()
			, back_inserter(resource.data.body)
			);
		resource.data.size = resources.at(i).Data.size();
	}

	EDAM::Types::Note enReplacement(noteStore.createNote(token, enNote));
	replacement.guid         = Guid(enReplacement.guid);
	replacement.name         = enReplacement.title;
	replacement.creationDate = static_cast<time_t>(enReplacement.created / 1000);
	replacement.usn          = enReplacement.updateSequenceNum;
	replacement.isDirty      = false;
}

void NoteStore::CreateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	EDAM::Types::Notebook enNotebook;
	enNotebook.__isset.name = true;

	enNotebook.name = notebook.name;

	EDAM::Types::Notebook enReplacement
		( noteStore.createNotebook(token, enNotebook)
		);
	replacement.guid    = enReplacement.guid;
	replacement.name    = enReplacement.name;
	replacement.usn     = enReplacement.updateSequenceNum;
	replacement.isDirty = false;
}

void NoteStore::CreateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	EDAM::Types::Tag enTag;
	enTag.__isset.name = true;

	enTag.name = tag.name;

	EDAM::Types::Tag enReplacement
		( noteStore.createTag(token, enTag)
		);
	replacement.guid    = enReplacement.guid;
	replacement.name    = enReplacement.name;
	replacement.usn     = enReplacement.updateSequenceNum;
	replacement.isDirty = false;
}

//------------------
// utility functions
//------------------

void NoteStore::ListEntries
	( EDAM::NoteStore::SyncChunk & chunk
	, EnInteropNoteList          & notes
	, NotebookList               & notebooks
	, TagList                    & tags
	)
{
	foreach (const EDAM::Types::Note & note, chunk.notes)
	{
		if (note.__isset.active && !note.active)
			continue;
		notes.push_back(EnInteropNote());

		notes.back().notebook = note.notebookGuid;

		notes.back().note.guid         = note.guid;
		notes.back().note.name         = note.title;
		notes.back().note.creationDate = static_cast<time_t>(note.created / 1000);
		notes.back().note.usn          = note.updateSequenceNum;
		notes.back().note.isDirty      = false;

		notes.back().guid    = notes.back().note.guid;
		notes.back().name    = notes.back().note.name;
		notes.back().usn     = notes.back().note.usn;
		notes.back().isDirty = notes.back().note.isDirty;

		// WARN: might be slow for large resource counts
		foreach (const EDAM::Types::Resource & resource, note.resources)
		{
			if (resource.noteGuid == note.guid)
				notes.back().resources.push_back(resource.guid);
		}
	}
	foreach (const EDAM::Types::Notebook & notebook, chunk.notebooks)
	{
		notebooks.push_back(Notebook());
		notebooks.back().guid    = notebook.guid;
		notebooks.back().name    = notebook.name;
		notebooks.back().usn     = notebook.updateSequenceNum;
		notebooks.back().isDirty = false;
	}
	foreach (const EDAM::Types::Tag & tag, chunk.tags)
	{
		tags.push_back(Tag());
		tags.back().guid    = tag.guid;
		tags.back().name    = tag.name;
		tags.back().usn     = tag.updateSequenceNum;
		tags.back().isDirty = false;
	}
}
