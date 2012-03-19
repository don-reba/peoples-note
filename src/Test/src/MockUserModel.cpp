#include "stdafx.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isCompacted     (false)
	, isInTransaction (false)
	, loadCount       (0)
	, loadMethod      (LoadMethodNone)
	, location        (DbLocationNone)
	, path            (L"card-path")
	, size            (42)
{
}

void MockUserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Guid     & notebook
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
	( const Guid & tag
	, const Note & note
	)
{
	foreach (const Tag & t, tags)
	{
		if (t.guid != tag)
			continue;
		noteTags.insert(make_pair(note.guid, tag));
		return;
	}
}

void MockUserModel::BeginTransaction()
{
	isInTransaction = true;
}

void MockUserModel::Compact()
{
	isCompacted = true;
}

void MockUserModel::DeleteNote(const Guid & note)
{
	deletedNotes.push_back(note);
}

void MockUserModel::DeleteNoteTags(const Guid & note)
{
	typedef multimap<Guid, Guid>::const_iterator Iter;
	pair<Iter, Iter> range(noteTags.equal_range(note));
	noteTags.erase(range.first, range.second);
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


void MockUserModel::GetDefaultNotebook(Guid & notebook)
{
	notebook = defaultNotebook;
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

int MockUserModel::GetDirtyNoteCount(const Guid & notebook)
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

void MockUserModel::GetLastUsedNotebook(Guid & notebook)
{
	notebook = lastUsedNotebook;
}

void MockUserModel::GetLastUsedNotebookOrDefault(Guid & notebook)
{
	notebook
		= lastUsedNotebook.IsEmpty()
		? defaultNotebook
		: lastUsedNotebook;
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

void MockUserModel::GetNoteAttachments
	( const Guid     & note
	, AttachmentList & attachments
	)
{
	attachments.clear();
	foreach (const Resource & r, resources)
	{
		if (r.FileName.empty() || r.Guid != note)
			continue;
		attachments.push_back(Attachment());
		Attachment & a(attachments.back());
		a.FileName = r.FileName;
		a.Guid     = r.Guid;
		a.Mime     = r.Mime;
	}
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
	struct EqualTag
	{
		const Guid & tag;
		EqualTag(const Guid & tag) : tag(tag) {}
		bool operator () (const pair<Guid, Guid> & p) { return p.second == tag; }
	};

	typedef multimap<Guid, Guid>::const_iterator Iter;

	foreach (Tag & tag, this->tags)
	{
		pair<Iter, Iter> range = noteTags.equal_range(note.guid);
		if (find_if(range.first, range.second, EqualTag(tag.guid)) != range.second)
			tags.push_back(tag);
	}
}

void MockUserModel::GetNotebook
	( const Guid & guid
	, Notebook   & notebook
	)
{
	if (guid == defaultNotebook)
	{
		notebook.name = L"default-notebook";
		notebook.guid = defaultNotebook;
		return;
	}
	if (guid == lastUsedNotebook)
	{
		notebook.name = L"last-used-notebook";
		notebook.guid = lastUsedNotebook;
		return;
	}
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

void MockUserModel::GetNotes
	( Guid       notebook // leave empty, if not used
	, wstring    search   // leave empty, if not used
	, int        start    // set count to 0, if not used
	, int        count    // set to 0, if not used
	, NoteList & notes
	)
{
	notebookSelection = notebook;
	searchSelection   = search;
	if (count > 0)
	{
		size_t finish(min(this->notes.size(), static_cast<size_t>(start + count)));
		notes.assign
			( this->notes.begin() + start
			, this->notes.begin() + finish
			);
	}
	else
	{
		notes.assign(this->notes.begin(), this->notes.end());
	}
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

void MockUserModel::GetResources(GuidList & resources)
{
	resources.clear();
	foreach (const Resource & r, this->resources)
		resources.push_back(r.Guid);
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

void MockUserModel::ReplaceNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Guid     & notebook
	)
{
	foreach (Note & n, notes)
	{
		if (n.guid != note.guid)
			continue;
		n = note;
		noteBodies[n.guid] = body;
		foreach (NoteRecord & nr, addedNotes)
		{
			if (nr.note.guid != note.guid)
				continue;
			nr = NoteRecord(note, body, bodyText, notebook);
			break;
		}
		return;
	}
	AddNote(note, body, bodyText, notebook);
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

void MockUserModel::UpdateResource
	( const Guid     & resource
	, const Resource & replacement
	)
{
	foreach (Resource & r, resources)
	{
		if (r.Guid != resource)
			continue;
		r = replacement;
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
