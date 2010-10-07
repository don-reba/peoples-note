#include "stdafx.h"
#include "NoteStore.h"

#include "Guid.h"
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
	( L"http://www.evernote.com/edam/note/"
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

void NoteStore::CreateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, const Guid             & notebook
	, Note                   & replacement
	)
{
	// initialize an Evernote note structure
	EDAM::Types::Note enNote;
	enNote.__isset.title        = true;
	enNote.__isset.created      = true;
	enNote.__isset.content      = true;
	enNote.__isset.resources    = true;
	enNote.__isset.notebookGuid = true;

	enNote.title        = note.name;
	enNote.created      = ConvertToEnTime(note.creationDate.GetTime());
	enNote.content      = body;
	enNote.notebookGuid = ConvertToUnicode(notebook);

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

	// call the createNote API and get the replacement note
	EDAM::Types::Note enReplacement(noteStore.createNote(token, enNote));

	// get data from the replacement Evernote note structure
	replacement.guid         = Guid(enReplacement.guid);
	replacement.name         = enReplacement.title;
	replacement.creationDate = static_cast<time_t>(ConvertFromEnTime(enReplacement.created));
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

void NoteStore::GetSyncState(SyncState & syncState)
{
	EDAM::NoteStore::SyncState enSyncState(noteStore.getSyncState(token));
	syncState.UpdateCount    = enSyncState.updateCount;
	syncState.FullSyncBefore = enSyncState.fullSyncBefore;
	syncState.CurrentEnTime  = enSyncState.currentTime;
}

void NoteStore::ListEntries
	( EnInteropNoteList & notes
	, NotebookList      & notebooks
	, TagList           & tags
	, const Guid        & notebookFilter
	)
{
	EDAM::NoteStore::SyncChunk chunk;
	chunk.chunkHighUSN = 0;
	chunk.updateCount  = 1;
	while (chunk.chunkHighUSN < chunk.updateCount)
	{
		chunk = noteStore.getSyncChunk(token, chunk.chunkHighUSN, 20, true);
		ListEntries(chunk, 0, notes, notebooks, tags, notebookFilter);
	}
}

void NoteStore::ListEntries
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
	EDAM::NoteStore::SyncChunk chunk;
	chunk.chunkHighUSN = notebookUpdateCount;
	chunk.updateCount  = notebookUpdateCount + 1;
	while (chunk.chunkHighUSN < chunk.updateCount)
	{
		chunk = noteStore.getSyncChunk(token, chunk.chunkHighUSN, 20, false);
		ListEntries
			( chunk
			, globalUpdateCount
			, notes
			, notebooks
			, tags
			, notebookFilter
			);
		ListEntries
			( chunk
			, expungedNotes
			, expungedNotebooks
			, expungedTags
			, resources
			, notebookFilter
			);
	}
}

void NoteStore::UpdateNote
	( const Note             & note
	, const wstring          & body
	, const vector<Resource> & resources
	, const Guid             & notebook
	, Note                   & replacement
	)
{
	EDAM::Types::Note enNote;
	enNote.__isset.guid         = true;
	enNote.__isset.title        = true;
	enNote.__isset.created      = true;
	enNote.__isset.content      = true;
	enNote.__isset.resources    = true;
	enNote.__isset.notebookGuid = true;

	enNote.guid         = ConvertToUnicode(note.guid);
	enNote.title        = note.name;
	enNote.created      = ConvertToEnTime(note.creationDate.GetTime());
	enNote.content      = body;
	enNote.notebookGuid = ConvertToUnicode(notebook);

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

	EDAM::Types::Note enReplacement(noteStore.updateNote(token, enNote));
	replacement.guid         = Guid(enReplacement.guid);
	replacement.name         = enReplacement.title;
	replacement.creationDate = static_cast<time_t>(ConvertFromEnTime(enReplacement.created));
	replacement.usn          = enReplacement.updateSequenceNum;
	replacement.isDirty      = false;
}

void NoteStore::UpdateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	EDAM::Types::Notebook enNotebook;
	enNotebook.__isset.guid = true;
	enNotebook.__isset.name = true;

	enNotebook.guid = ConvertToUnicode(notebook.guid);
	enNotebook.name = notebook.name;

	int replacementUsn(noteStore.updateNotebook(token, enNotebook));
	replacement.guid    = notebook.guid;
	replacement.name    = notebook.name;
	replacement.usn     = replacementUsn;
	replacement.isDirty = false;
}

void NoteStore::UpdateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	EDAM::Types::Tag enTag;
	enTag.__isset.guid = true;
	enTag.__isset.name = true;

	enTag.guid = ConvertToUnicode(tag.guid);
	enTag.name = tag.name;

	int replacementUsn(noteStore.updateTag(token, enTag));
	replacement.guid    = tag.guid;
	replacement.name    = tag.name;
	replacement.usn     = replacementUsn;
	replacement.isDirty = false;
}

//------------------
// utility functions
//------------------

__int64 NoteStore::ConvertFromEnTime(__int64 enTime)
{
	return static_cast<time_t>(enTime / 1000L);
}

__int64 NoteStore::ConvertToEnTime(__int64 time)
{
	return static_cast<__int64>(time) * 1000L;
}

void NoteStore::ListEntries
	( EDAM::NoteStore::SyncChunk & chunk
	, int                          globalUpdateCount
	, EnInteropNoteList          & notes
	, NotebookList               & notebooks
	, TagList                    & tags
	, const Guid                 & notebookFilter
	)
{
	wstring notebookFilterGuid(ConvertToUnicode(notebookFilter));
	if (!notebookFilter.IsLocal())
	{
		foreach (const EDAM::Types::Note & note, chunk.notes)
		{
			if (note.__isset.active && !note.active)
				continue;
			if (note.notebookGuid != notebookFilterGuid)
				continue;
			notes.push_back(EnInteropNote());

			notes.back().notebook = note.notebookGuid;

			notes.back().note.guid         = note.guid;
			notes.back().note.name         = note.title;
			notes.back().note.creationDate = static_cast<time_t>(ConvertFromEnTime(note.created));
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
	}
	foreach (const EDAM::Types::Notebook & notebook, chunk.notebooks)
	{
		if (notebook.updateSequenceNum <= globalUpdateCount)
			continue;
		notebooks.push_back(Notebook());
		notebooks.back().guid    = notebook.guid;
		notebooks.back().name    = notebook.name;
		notebooks.back().usn     = notebook.updateSequenceNum;
		notebooks.back().isDirty = false;
	}
	foreach (const EDAM::Types::Tag & tag, chunk.tags)
	{
		if (tag.updateSequenceNum <= globalUpdateCount)
			continue;
		tags.push_back(Tag());
		tags.back().guid    = tag.guid;
		tags.back().name    = tag.name;
		tags.back().usn     = tag.updateSequenceNum;
		tags.back().isDirty = false;
	}
}

void NoteStore::ListEntries
	( EDAM::NoteStore::SyncChunk & chunk
	, vector<Guid>               & expungedNotes
	, vector<Guid>               & expungedNotebooks
	, vector<Guid>               & expungedTags
	, vector<Guid>               & resources
	, const Guid                 & notebookFilter
	)
{
	wstring notebookFilterGuid(ConvertToUnicode(notebookFilter));
	foreach (const EDAM::Types::Note & note, chunk.notes)
	{
		if (!note.__isset.active || note.active)
			continue;
		if (note.notebookGuid != notebookFilterGuid)
			continue;
		expungedNotes.push_back(note.guid);
	}
	foreach (const EDAM::Types::Resource & resource, chunk.resources)
		resources.push_back(resource.guid);
	foreach (const EDAM::Types::Guid & guid, chunk.expungedNotes)
		expungedNotes.push_back(guid);
	foreach (const EDAM::Types::Guid & guid, chunk.expungedNotebooks)
		expungedNotebooks.push_back(guid);
	foreach (const EDAM::Types::Guid & guid, chunk.expungedTags)
		expungedTags.push_back(guid);
}
