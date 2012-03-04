#include "stdafx.h"
#include "NoteStore.h"

#include "EnRecognitionParser.h"
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
	, GuidList               & replacementTags
	)
{
	EDAM::Type::Note enNote;
	ConvertToEnNote(note, body, notebook, enNote);

	enNote.__isset.resources = true;
	enNote.resources.resize(resources.size());
	for (int i(0); i != resources.size(); ++i)
		ConvertToEnResource(resources.at(i), enNote.resources.at(i));

	ConvertFromEnNote
		( noteStore.createNote(token, enNote)
		, replacement
		, replacementTags
		);
}

void NoteStore::CreateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	EDAM::Type::Notebook enNotebook;
	enNotebook.__isset.name = true;

	enNotebook.name = notebook.name;

	EDAM::Type::Notebook enReplacement(noteStore.createNotebook(token, enNotebook));
	ConvertFromEnNotebook(enReplacement, replacement);
}

void NoteStore::CreateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	EDAM::Type::Tag enTag;
	ConvertToEnTag(tag, enTag);

	ConvertFromEnTag
		( noteStore.createTag(token, enTag)
		, replacement
		);
}

void NoteStore::DeleteNote(const Guid & guid)
{
	noteStore.deleteNote(token, guid);
}

void NoteStore::GetDefaultNotebook(Guid & notebook)
{
	EDAM::Type::Notebook enNotebook(noteStore.getDefaultNotebook(token));
	notebook = Guid(enNotebook.guid);
}

void NoteStore::GetNote
	( const Guid & guid
	,       Note & note
	, GuidList   & tags
	)
{
	ConvertFromEnNote
		( noteStore.getNote
			( token // authenticationToken
			, guid  // guid
			, false // withContent
			, false // withResourceData
			, false // withResourcesRecognition
			, false // withResourcesAlternateData
			)
		, note
		, tags
		);
}

void NoteStore::GetNoteBody
	( const Note & note
	, wstring    & content
	)
{
	content = noteStore.getNoteContent(token, note.guid);
}

void NoteStore::GetNoteResource
	( const Guid           & guid
	, Resource             & resource
	, RecognitionEntryList & recognitionEntries
	)
{
	ConvertFromEnResource
		( noteStore.getResource
			( token // authenticationToken
			, guid  // guid
			, true  // withData
			, true  // withRecognition
			, true  // withAttributes
			, false // withAlternateData
			)
		, guid
		, resource
		, recognitionEntries
		);
}

void NoteStore::GetNoteTagNames
	( const Note      & note
	, vector<wstring> & names
	)
{
	names = noteStore.getNoteTagNames(token, note.guid);
}

void NoteStore::GetNotebook
	( const Guid     & guid
	,       Notebook & notebook
	)
{
	ConvertFromEnNotebook
		( noteStore.getNotebook
			( token // authenticationToken
			, guid  // guid
			)
		, notebook
		);
}

void NoteStore::GetResource(const Guid & guid, Resource & resource)
{
	ConvertFromEnResource
		( noteStore.getResource
			( token // authenticationToken
			, guid  // guid
			, false // withData
			, false // withRecognition
			, true  // withAttributes
			, false // withAlternateData
			)
		, resource
		);
}

void NoteStore::GetSyncState(SyncState & syncState)
{
	EDAM::NoteStore::SyncState enSyncState(noteStore.getSyncState(token));
	syncState.UpdateCount    = enSyncState.updateCount;
	syncState.FullSyncBefore = enSyncState.fullSyncBefore;
	syncState.CurrentEnTime  = enSyncState.currentTime;
}

void NoteStore::GetTag
	( const Guid & guid
	,       Tag  & tag
	)
{
	ConvertFromEnTag
		( noteStore.getTag
			( token // authenticationToken
			, guid  // guid
			)
		, tag
		);
}

void NoteStore::ListFullSyncEntries
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
		chunk = noteStore.getSyncChunk(token, chunk.chunkHighUSN, 150, true);
		ListEntries(chunk, 0, notes, notebooks, tags, notebookFilter);
	}
}

void NoteStore::ListIncrementalSyncEntries
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
	( const Note         & note
	, const wstring      & body
	, const GuidList     & tags
	, const ResourceList & resources
	, const Guid         & notebook
	, Note               & replacement
	, GuidList           & replacementTags
	)
{
	const size_t tagLimit(EDAM::Limits::constants.EDAM_NOTE_TAGS_MAX);

	EDAM::Type::Note enNote;
	ConvertToEnNote(note, body, notebook, enNote);

	enNote.__isset.tagGuids = true;
	copy(tags.begin(), tags.end(), back_inserter(enNote.tagGuids));
	if (enNote.tagGuids.size() > tagLimit)
		enNote.tagGuids.resize(tagLimit);

	enNote.__isset.resources = true;
	enNote.resources.resize(resources.size());
	for (int i(0); i != resources.size(); ++i)
		ConvertToEnResource(resources.at(i), enNote.resources.at(i));

	ConvertFromEnNote
		( noteStore.updateNote(token, enNote)
		, replacement
		, replacementTags
		);
}

void NoteStore::UpdateNotebook
	( const Notebook & notebook
	, Notebook       & replacement
	)
{
	EDAM::Type::Notebook enNotebook;
	ConvertToEnNotebook(notebook, enNotebook);

	replacement = notebook;
	replacement.usn     = noteStore.updateNotebook(token, enNotebook);
	replacement.isDirty = false;
}

void NoteStore::UpdateTag
	( const Tag & tag
	, Tag       & replacement
	)
{
	EDAM::Type::Tag enTag;
	ConvertToEnTag(tag, enTag);

	int replacementUsn(noteStore.updateTag(token, enTag));
	replacement = tag;
	replacement.usn     = replacementUsn;
	replacement.isDirty = false;
}

//------------------
// utility functions
//------------------

Timestamp NoteStore::ConvertFromEnTime(__int64 enTime)
{
	return Timestamp(static_cast<time_t>(enTime / 1000L));
}

__int64 NoteStore::ConvertToEnTime(Timestamp time)
{
	return time.GetTime() * 1000L;
}

void NoteStore::ConvertFromEnNote
	( const EDAM::Type::Note & enNote
	, Note                   & note
	, GuidList               & tags
	)
{
	note.guid             = Guid(enNote.guid);
	note.creationDate     = ConvertFromEnTime(enNote.created);
	note.modificationDate = ConvertFromEnTime(enNote.updated);
	note.name             = enNote.title;
	note.usn              = enNote.updateSequenceNum;
	note.isDirty          = false;
	if (enNote.__isset.tagGuids)
	{
		tags.clear();
		tags.reserve(enNote.tagGuids.size());
		foreach (const EDAM::Type::Guid & guid, enNote.tagGuids)
			tags.push_back(Guid(guid));
	}
	if (enNote.__isset.attributes)
	{
		const EDAM::Type::NoteAttributes & attributes(enNote.attributes);
		if (attributes.__isset.subjectDate)
			note.subjectDate = ConvertFromEnTime(attributes.subjectDate);
		if (attributes.__isset.latitude && attributes.__isset.longitude && attributes.__isset.altitude)
			note.Location = Location(attributes.altitude, attributes.latitude, attributes.longitude);
		if (attributes.__isset.author)
			note.Author = attributes.author;
		if (attributes.__isset.source)
			note.Source = attributes.source;
		if (attributes.__isset.sourceURL)
			note.SourceUrl = attributes.sourceURL;
		if (attributes.__isset.sourceApplication)
			note.SourceApplication = attributes.sourceApplication;
	}
}

void NoteStore::ConvertToEnNote
	( const Note       & note
	, const wstring    & body
	, const Guid       & notebook
	, EDAM::Type::Note & enNote
	)
{
	enNote.__isset.guid         = true;
	enNote.__isset.title        = true;
	enNote.__isset.created      = true;
	enNote.__isset.content      = true;
	enNote.__isset.notebookGuid = true;

	enNote.guid         = note.guid;
	enNote.title        = note.name;
	enNote.created      = ConvertToEnTime(note.creationDate);
	enNote.updated      = ConvertToEnTime(note.modificationDate);
	enNote.content      = body;
	enNote.notebookGuid = notebook;

	// set optional attributes
	if (note.subjectDate.GetTime())
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.subjectDate = true;
		enNote.attributes.subjectDate = ConvertToEnTime(note.subjectDate);
	}
	if (note.Location.IsValid)
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.altitude  = true;
		enNote.attributes.__isset.longitude = true;
		enNote.attributes.__isset.latitude  = true;
		enNote.attributes.altitude  = note.Location.Altitude;
		enNote.attributes.longitude = note.Location.Longitude;
		enNote.attributes.latitude  = note.Location.Latitude;
	}
	if (!note.Author.empty())
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.author = true;
		enNote.attributes.author = note.Author;
	}
	if (!note.Source.empty())
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.source = true;
		enNote.attributes.source = note.Source;
	}
	if (!note.SourceUrl.empty())
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.sourceURL = true;
		enNote.attributes.sourceURL = note.SourceUrl;
	}
	if (!note.SourceApplication.empty())
	{
		enNote.__isset.attributes = true;
		enNote.attributes.__isset.sourceApplication = true;
		enNote.attributes.sourceApplication = note.SourceApplication;
	}
}

void NoteStore::ConvertFromEnNotebook
	( const EDAM::Type::Notebook & enNotebook
	, Notebook                   & notebook
	)
{
	notebook.guid             = Guid(enNotebook.guid);
	notebook.name             = enNotebook.name;
	notebook.CreationDate     = ConvertFromEnTime(enNotebook.serviceCreated);
	notebook.ModificationDate = ConvertFromEnTime(enNotebook.serviceUpdated);
	notebook.usn              = enNotebook.updateSequenceNum;
	notebook.isDirty          = false;
}

void NoteStore::ConvertToEnNotebook
	( const Notebook       & notebook
	, EDAM::Type::Notebook & enNotebook
	)
{
	enNotebook.__isset.guid = true;
	enNotebook.__isset.name = true;

	enNotebook.guid = notebook.guid;
	enNotebook.name = notebook.name;
}

void NoteStore::ConvertFromEnResource
	( const EDAM::Type::Resource & enResource
	, Resource                   & resource
	)
{
	resource.Hash = HashWithMD5(resource.Data);
	resource.Guid = Guid(enResource.guid);
	resource.Note = Guid(enResource.noteGuid);
	resource.Mime = enResource.mime;
	resource.IsAttachment = false;
	if (enResource.__isset.width && enResource.__isset.height)
	{
		resource.Dimensions.Width  = enResource.width;
		resource.Dimensions.Height = enResource.height;
		resource.Dimensions.IsValid = true;
	}
	if (enResource.__isset.attributes)
	{
		const EDAM::Type::ResourceAttributes & attributes(enResource.attributes);
		if (attributes.__isset.sourceURL)
			resource.SourceUrl = attributes.sourceURL;
		if (attributes.__isset.timestamp)
			resource.Timestamp = ConvertFromEnTime(attributes.timestamp);
		if (attributes.__isset.latitude && attributes.__isset.longitude && attributes.__isset.altitude)
			resource.Location = Location(attributes.altitude, attributes.latitude, attributes.longitude);
		if (attributes.__isset.fileName)
			resource.FileName = attributes.fileName;
		if (attributes.__isset.attachment)
			resource.IsAttachment = attributes.attachment;
	}
}

void NoteStore::ConvertFromEnResource
	( const EDAM::Type::Resource & enResource
	, const Guid                 & guid
	, Resource                   & resource
	, RecognitionEntryList       & recognitionEntries
	)
{
	resource.Data.reserve(enResource.data.body.size());
	if (resource.Data.capacity() < enResource.data.body.size())
		throw std::exception("Not enough memory to download resource.");
	copy
		( enResource.data.body.begin()
		, enResource.data.body.end()
		, back_inserter(resource.Data)
		);
	resource.Hash = HashWithMD5(resource.Data);
	resource.Guid = Guid(enResource.guid);
	resource.Note = Guid(enResource.noteGuid);
	resource.Mime = enResource.mime;
	resource.IsAttachment = false;
	if (enResource.__isset.width && enResource.__isset.height)
	{
		resource.Dimensions.Width  = enResource.width;
		resource.Dimensions.Height = enResource.height;
		resource.Dimensions.IsValid = true;
	}
	if (enResource.__isset.attributes)
	{
		const EDAM::Type::ResourceAttributes & attributes(enResource.attributes);
		if (attributes.__isset.sourceURL)
			resource.SourceUrl = attributes.sourceURL;
		if (attributes.__isset.timestamp)
			resource.Timestamp = ConvertFromEnTime(attributes.timestamp);
		if (attributes.__isset.latitude && attributes.__isset.longitude && attributes.__isset.altitude)
			resource.Location = Location(attributes.altitude, attributes.latitude, attributes.longitude);
		if (attributes.__isset.fileName)
			resource.FileName = attributes.fileName;
		if (attributes.__isset.attachment)
			resource.IsAttachment = attributes.attachment;
	}

	if (enResource.__isset.recognition)
	{
		TBinary recognition;
		recognition.reserve(enResource.recognition.body.size() + 1);
		copy
			( enResource.recognition.body.begin()
			, enResource.recognition.body.end()
			, back_inserter(recognition)
			);
		recognition.push_back(L'\0');

		wstring info;
		ConvertToUnicode(&recognition.at(0), info);

		EnRecognitionParser parser;
		parser.Parse(info, recognitionEntries, guid);
	}
}

void NoteStore::ConvertToEnResource
	( const Resource       & resource
	, EDAM::Type::Resource & enResource
	)
{
	enResource.__isset.guid = true;
	enResource.guid = resource.Guid;

	enResource.__isset.data = true;
	enResource.data.__isset.body = true;
	enResource.data.__isset.size = true;

	copy
		( resource.Data.begin()
		, resource.Data.end()
		, back_inserter(enResource.data.body)
		);
	enResource.data.size = resource.Data.size();

	enResource.__isset.mime = true;
	enResource.mime = L"image/jpeg";

	if (resource.Dimensions.IsValid)
	{
		enResource.__isset.width  = true;
		enResource.__isset.height = true;
		enResource.width  = resource.Dimensions.Width;
		enResource.height = resource.Dimensions.Height;
	}
	if (!resource.SourceUrl.empty())
	{
		enResource.__isset.attributes = true;
		enResource.attributes.__isset.sourceURL = true;
		enResource.attributes.sourceURL = resource.SourceUrl;
	}
	if (resource.Timestamp.GetTime() != 0)
	{
		enResource.__isset.attributes = true;
		enResource.attributes.__isset.timestamp = true;
		enResource.attributes.timestamp = ConvertToEnTime(resource.Timestamp);
	}
	if (resource.Location.IsValid)
	{
		enResource.__isset.attributes = true;
		enResource.attributes.__isset.altitude  = true;
		enResource.attributes.__isset.longitude = true;
		enResource.attributes.__isset.latitude  = true;
		enResource.attributes.altitude  = resource.Location.Altitude;
		enResource.attributes.longitude = resource.Location.Longitude;
		enResource.attributes.latitude  = resource.Location.Latitude;
	}
	if (!resource.FileName.empty())
	{
		enResource.__isset.attributes = true;
		enResource.attributes.__isset.fileName = true;
		enResource.attributes.fileName = resource.FileName;
	}
	if (resource.IsAttachment)
	{
		enResource.__isset.attributes = true;
		enResource.attributes.__isset.attachment = true;
		enResource.attributes.attachment = resource.IsAttachment;
	}
}

void NoteStore::ConvertFromEnTag
	( const EDAM::Type::Tag & enTag
	, Tag                   & tag
	)
{
	tag.name    = enTag.name;
	tag.guid    = Guid(enTag.guid);
	tag.usn     = enTag.updateSequenceNum;
	tag.isDirty = false;

	if (enTag.__isset.parentGuid)
		tag.parentGuid = Guid(enTag.parentGuid);
	else
		tag.parentGuid = Guid::GetEmpty();
}

void NoteStore::ConvertToEnTag
	( const Tag       & tag
	, EDAM::Type::Tag & enTag
	)
{
	enTag.__isset.guid = true;
	enTag.__isset.name = true;

	enTag.guid = tag.guid;
	enTag.name = tag.name;

	if (!tag.parentGuid.IsEmpty())
	{
		enTag.__isset.parentGuid = true;
		enTag.parentGuid = tag.parentGuid;
	}
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
	wstring notebookFilterGuid(notebookFilter);
	if (!notebookFilter.IsLocal())
	{
		int noteCount(0);
		foreach (const EDAM::Type::Note & note, chunk.notes)
		{
			if (note.__isset.active && !note.active)
				continue;
			if (!notebookFilter.IsEmpty() && note.notebookGuid != notebookFilterGuid)
				continue;
			++noteCount;
		}
		notes.reserve(notes.size() + noteCount);

		foreach (const EDAM::Type::Note & note, chunk.notes)
		{
			if (note.__isset.active && !note.active)
				continue;
			if (!notebookFilter.IsEmpty() && note.notebookGuid != notebookFilterGuid)
				continue;

			Note     newNote;
			GuidList newNoteTags;
			ConvertFromEnNote(note, newNote, newNoteTags);

			notes.push_back(EnInteropNote(newNote, Guid(note.notebookGuid)));
			EnInteropNote & enNote(notes.back());

			copy
				( newNoteTags.begin()
				, newNoteTags.end()
				, back_inserter(notes.back().tags)
				);

			int resourceCount(0);
			foreach (const EDAM::Type::Resource & resource, note.resources)
			{
				if (resource.noteGuid == note.guid)
					++resourceCount;
			}
			enNote.resources.reserve(resourceCount);

			foreach (const EDAM::Type::Resource & resource, note.resources)
			{
				if (resource.noteGuid == note.guid)
					enNote.resources.push_back(Guid(resource.guid));
			}
		}
	}
	foreach (const EDAM::Type::Notebook & notebook, chunk.notebooks)
	{
		if (notebook.updateSequenceNum <= globalUpdateCount)
			continue;
		notebooks.push_back(Notebook());
		ConvertFromEnNotebook(notebook, notebooks.back());
	}
	foreach (const EDAM::Type::Tag & tag, chunk.tags)
	{
		if (tag.updateSequenceNum <= globalUpdateCount)
			continue;
		tags.push_back(Tag());
		tags.back().guid    = Guid(tag.guid);
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
	wstring notebookFilterGuid(notebookFilter);
	foreach (const EDAM::Type::Note & note, chunk.notes)
	{
		if (!note.__isset.active || note.active)
			continue;
		if (!notebookFilter.IsEmpty() && note.notebookGuid != notebookFilterGuid)
			continue;
		expungedNotes.push_back(Guid(note.guid));
	}
	foreach (const EDAM::Type::Resource & resource, chunk.resources)
		resources.push_back(Guid(resource.guid));
	foreach (const EDAM::Type::Guid & guid, chunk.expungedNotes)
		expungedNotes.push_back(Guid(guid));
	foreach (const EDAM::Type::Guid & guid, chunk.expungedNotebooks)
		expungedNotebooks.push_back(Guid(guid));
	foreach (const EDAM::Type::Guid & guid, chunk.expungedTags)
		expungedTags.push_back(Guid(guid));
}
