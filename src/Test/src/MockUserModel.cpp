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
	defaultNotebook.name  = L"default-notebook";
	lastUsedNotebook.name = L"last-used-notebook";
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

void MockUserModel::DeleteNotebook(const Guid & notebook)
{
	deletedNotebooks.push_back(notebook);
}

void MockUserModel::DeleteTag(const Guid & tag)
{
	deletedTags.push_back(tag);
}

void MockUserModel::EndTransaction()
{
	isInTransaction = false;
}

bool MockUserModel::Exists(const wstring & username)
{
	return validUsernames.find(username) != validUsernames.end();
}

void MockUserModel::GetCredentials(Credentials & credentials)
{
	credentials = this->credentials;
}

void MockUserModel::GetDefaultNotebook(Notebook & notebook)
{
	notebook = defaultNotebook;
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
	notebook = lastUsedNotebook;
}

DbLocation MockUserModel::GetLocation()
{
	return location;
}

Note MockUserModel::GetNote(Guid guid)
{
	const string & guidString = static_cast<const string &>(guid);
	foreach (Note & note, notes)
	{
		if (guidString == static_cast<const string &>(note.guid))
			return note;
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

__int64 MockUserModel::GetSize()
{
	return size;
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

void MockUserModel::GetTags(TagList & tags)
{
	copy(this->tags.begin(), this->tags.end(), back_inserter(tags));
}

int MockUserModel::GetUpdateCount()
{
	return updateCount;
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

void MockUserModel::MakeNotebookDefault(const Notebook & notebook)
{
	defaultNotebook = notebook;
}

void MockUserModel::MakeNotebookLastUsed(const Notebook & notebook)
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

void MockUserModel::SetCredentials
		( const wstring & username
		, const wstring & password
		)
{
	credentials.SetUsername(username);
	credentials.SetPassword(password);
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

void MockUserModel::SetUpdateCount(int updateCount)
{
	this->updateCount = updateCount;
}

void MockUserModel::Unload()
{
	loadMethod = LoadMethodNone;
	loadedAs.clear();
}

void MockUserModel::UpdateNotebook
	( const Notebook & notebook
	, const Notebook & replacement
	)
{
	foreach (Notebook & n, notebooks)
	{
		if (n.guid != notebook.guid)
			continue;
		n = replacement;
		return;
	}
}

void MockUserModel::UpdateTag
	( const Tag & tag
	, const Tag & replacement
	)
{
	foreach (Tag & t, tags)
	{
		if (t.guid != tag.guid)
			continue;
		t = replacement;
		return;
	}
}
