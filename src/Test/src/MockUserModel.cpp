#include "stdafx.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isInTransaction  (false)
	, loadCount        (0)
	, loadMethod       (LoadMethodNone)
	, location         (DbLocationNone)
	, path             (L"card-path")
	, size             (42)
{
}

void MockUserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Notebook & notebook
	)
{
	addedNotes.push_back(NoteRecord(note, body, bodyText, notebook));
	notes.push_back(note);
	noteBodies[note.guid] = body;
}

void MockUserModel::AddNotebook(const Notebook & notebook)
{
	notebooks.push_back(notebook);
}

void MockUserModel::AddRecognitionEntry(const RecognitionEntry & entry)
{
	recognitionEntries.push_back(entry);
}

void MockUserModel::AddResource(const Resource & resource)
{
	resources.push_back(resource);
}

void MockUserModel::AddTag(const Tag & tag)
{
	tags.push_back(tag);
}

void MockUserModel::AddTagToNote
	( const std::wstring & tagName
	, const Note         & note
	)
{
	foreach (const Tag & tag, tags)
	{
		if (tag.name == tagName)
		{
			noteTags.insert(NoteTag(note.guid, tag.guid));
			return;
		}
	}
}

void MockUserModel::BeginTransaction()
{
	isInTransaction = true;
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void MockUserModel::DeleteNote(const Guid & note)
{
	deletedNotes.push_back(note);
}

void MockUserModel::DeleteNoteThumbnail(const Guid & note)
{
	noteThumbnails.erase(note);
}

void MockUserModel::EndTransaction()
{
	isInTransaction = false;
}

bool MockUserModel::Exists(const wstring & username)
{
	return validUsernames.find(username) != validUsernames.end();
}

void MockUserModel::ExpungeNote(const Guid & note)
{
	expungedNotes.push_back(note);
}

void MockUserModel::ExpungeNotebook(const Guid & notebook)
{
	expungedNotebooks.push_back(notebook);
}

void MockUserModel::ExpungeTag(const Guid & tag)
{
	expungedTags.push_back(tag);
}

void MockUserModel::GetDefaultNotebook(Notebook & notebook)
{
	notebook.guid = defaultNotebook;
	notebook.name = L"default-notebook";
}

void MockUserModel::GetDeletedNotes(GuidList & notes)
{
	notes.clear();
	copy
		( deletedNotes.begin()
		, deletedNotes.end()
		, back_inserter(notes)
		);
}

int MockUserModel::GetDirtyNoteCount(const Notebook & notebook)
{
	int count(0);
	foreach (const Note & note, notes)
	{
		if (note.isDirty)
			++count;
	}
	return count;
}

wstring MockUserModel::GetFolder()
{
	return folder;
}

__int64 MockUserModel::GetLastSyncEnTime()
{
	return lastSyncEnTime;
}

void MockUserModel::GetLastUsedNotebook(Notebook & notebook)
{
	notebook.guid = lastUsedNotebook;
	notebook.name = L"last-used-notebook";
}

DbLocation MockUserModel::GetLocation()
{
	return location;
}

void MockUserModel::GetNote(const Guid & guid, Note & note)
{
	const string & guidString = static_cast<const string &>(guid);
	foreach (Note & n, notes)
	{
		if (guidString == static_cast<const string &>(n.guid))
		{
			note = n;
			return;
		}
	}
	throw std::exception("Note not found.");
}

void MockUserModel::GetNoteBody
	( const Guid & guid
	, wstring    & resource
	)
{
	if (noteBodies.find(guid) != noteBodies.end())
		resource = noteBodies[guid];
}

void MockUserModel::GetNoteResources
	( const Guid   & note
	, vector<Guid> & resources
	)
{
	foreach (Resource & resource, this->resources)
	{
		if (resource.Note == note)
			resources.push_back(resource.Guid);
	}
}

void MockUserModel::GetNoteTags
	( const Note & note
	, TagList    & tags
	)
{
	NoteTag noteTag;
	noteTag.note = note.guid;
	foreach (Tag & tag, this->tags)
	{
		noteTag.tag = tag.guid;
		if (noteTags.find(noteTag) != noteTags.end())
			tags.push_back(tag);
	}
}

void MockUserModel::GetNotebook
	( const Guid & guid
	, Notebook   & notebook
	)
{
	foreach (const Notebook & n, notebooks)
	{
		if (n.guid == guid)
		{
			notebook = n;
			return;
		}
	}
}

void MockUserModel::GetNotebooks(NotebookList & notebooks)
{
	copy
		( this->notebooks.begin()
		, this->notebooks.end()
		, back_inserter(notebooks)
		);
}

int MockUserModel::GetNotebookUpdateCount(const Guid & notebook)
{
	return notebookUpdateCounts[notebook];
}

void MockUserModel::GetNotesByNotebook
	( const Notebook & notebook
	, NoteList       & notes
	)
{
	notebookSelection = notebook.name;
	notes.clear();
	copy(this->notes.begin(), this->notes.end(), back_inserter(notes));
}

void MockUserModel::GetNotesBySearch
	( const std::wstring & search
	, NoteList           & notes
	)
{
	searchSelection = search;
	notes.clear();
	copy(this->notes.begin(), this->notes.end(), back_inserter(notes));
}

void MockUserModel::GetNoteThumbnail
	( const Guid & guid
	, Thumbnail & thumbnail)
{
	if (noteThumbnails.end() != noteThumbnails.find(guid))
		thumbnail = noteThumbnails[guid];
}

wstring MockUserModel::GetPath()
{
	return path;
}

wstring MockUserModel::GetPasswordHash()
{
	return password;
}

void MockUserModel::GetResource
	( const string & hash
	, Blob         & blob
	)
{
	foreach (const Resource & resource, resources)
	{
		if (resource.Hash == hash)
			blob = resource.Data;
	}
}

void MockUserModel::GetResource(const Guid & guid, Resource & resource)
{
	foreach (const Resource & r, resources)
	{
		if (r.Guid == guid)
			resource = r;
	}
}

__int64 MockUserModel::GetSize()
{
	return size;
}

int MockUserModel::GetSyncVersion()
{
	return syncVersion;
}

void MockUserModel::GetTags(TagList & tags)
{
	copy(this->tags.begin(), this->tags.end(), back_inserter(tags));
}

int MockUserModel::GetUpdateCount()
{
	return updateCount;
}

wstring MockUserModel::GetUsername()
{
	return username;
}

int MockUserModel::GetVersion()
{
	return version;
}

void MockUserModel::Load(const wstring & username)
{
	if (loadMethod != LoadMethodNone)
	{
		loadMethod = LoadMethodNone;
		return;
	}
	++loadCount;
	loadedAs   = username;
	loadMethod = LoadMethodLoad;
}

void MockUserModel::LoadAs
		( const wstring & oldUsername
		, const wstring & newUsername
		)
{
	if (loadMethod != LoadMethodNone)
	{
		loadMethod = LoadMethodNone;
		return;
	}
	++loadCount;
	loadedAs   = newUsername;
	loadMethod = LoadMethodLoadAs;
}

void MockUserModel::LoadOrCreate(const wstring & username)
{
	if (loadMethod != LoadMethodNone)
	{
		loadMethod = LoadMethodNone;
		return;
	}
	++loadCount;
	loadedAs   = username;
	loadMethod = LoadMethodLoadOrCreate;
}

void MockUserModel::MakeNotebookDefault(const Guid & notebook)
{
	defaultNotebook = notebook;
}

void MockUserModel::MakeNotebookLastUsed(const Guid & notebook)
{
	lastUsedNotebook = notebook;
}

void MockUserModel::MoveToCard()
{
	location = DbLocationCard;
	path     = L"card-path";
}

void MockUserModel::MoveToDevice()
{
	location = DbLocationDevice;
	path     = L"device-path";
}

void MockUserModel::RemoveNoteTags(const Guid & note)
{
	set<NoteTag>::iterator i(noteTags.begin());
	while (i != noteTags.end())
	{
		set<NoteTag>::iterator next(i);
		++next;
		if (note == i->note)
			noteTags.erase(i);
		i = next;
	}
}

void MockUserModel::SetCredentials
		( const wstring & username
		, const wstring & passwordHash
		)
{
	this->username = username;
	this->password = passwordHash;
}

void MockUserModel::SetLastSyncEnTime(__int64 enTime)
{
	lastSyncEnTime = enTime;
}

void MockUserModel::SetNoteThumbnail
	( const Guid      & guid
	, const Thumbnail & thumbnail
	)
{
	noteThumbnails[guid] = thumbnail;
}

void MockUserModel::SetNotebookUpdateCount
	( const Guid & notebook
	, int          updateCount
	)
{
	notebookUpdateCounts[notebook] = updateCount;
}

void MockUserModel::SetSyncVersion(int version)
{
	syncVersion = version;
}

void MockUserModel::SetUpdateCount(int updateCount)
{
	this->updateCount = updateCount;
}

void MockUserModel::Unload()
{
	loadMethod = LoadMethodNone;
	loadedAs.clear();
}

void MockUserModel::UpdateNote
	( const Guid & note
	, const Note & replacement
	)
{
	foreach (Note & n, notes)
	{
		if (n.guid != note)
			continue;
		n = replacement;
		return;
	}
}

void MockUserModel::UpdateNotebook
	( const Guid     & notebook
	, const Notebook & replacement
	)
{
	foreach (Notebook & n, notebooks)
	{
		if (n.guid != notebook)
			continue;
		n = replacement;
		return;
	}
}

void MockUserModel::UpdateTag
	( const Guid & tag
	, const Tag  & replacement
	)
{
	foreach (Tag & t, tags)
	{
		if (t.guid != tag)
			continue;
		t = replacement;
		return;
	}
}
