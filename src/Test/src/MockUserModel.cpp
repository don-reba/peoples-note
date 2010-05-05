#include "stdafx.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isInTransaction  (false)
	, loadMethod       (LoadMethodNone)
{
	defaultNotebook.name  = L"default-notebook";
	lastUsedNotebook.name = L"last-used-notebook";
}

void MockUserModel::AddImageResource
	( string       hash
	, const Blob & data
	, const Guid & noteGuid
	)
{
	addedImages.push_back(ImageRecord(hash, data, noteGuid));
	images[hash] = data;
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

void MockUserModel::AddTag(const Tag & tag)
{
	tags.push_back(tag);
}

void MockUserModel::BeginTransaction()
{
	isInTransaction = true;
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void MockUserModel::DeleteNote(const Note & note)
{
	deletedNotes.push_back(note);
}

void MockUserModel::DeleteNotebook(const Notebook & notebook)
{
	deletedNotebooks.push_back(notebook);
}

void MockUserModel::DeleteTag(const Tag & tag)
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

wstring MockUserModel::GetFolder() const
{
	return folder;
}

void MockUserModel::GetImageResource(string hash, Blob & blob)
{
	if (images.find(hash) != images.end())
		blob = images[hash];
}

void MockUserModel::GetLastUsedNotebook(Notebook & notebook)
{
	notebook = lastUsedNotebook;
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

void MockUserModel::GetNoteBody(Guid guid, wstring & resource)
{
	if (noteBodies.find(guid) != noteBodies.end())
		resource = noteBodies[guid];
}

void MockUserModel::GetNoteThumbnail
	( const Guid & guid
	, Thumbnail & thumbnail)
{
	if (noteThumbnails.end() != noteThumbnails.find(guid))
		thumbnail = noteThumbnails[guid];
}

void MockUserModel::GetNotebooks(NotebookList & notebooks)
{
	copy
		( this->notebooks.begin()
		, this->notebooks.end()
		, back_inserter(notebooks)
		);
}

void MockUserModel::GetNotesByNotebook
	( const Notebook & notebook
	, NoteList       & notes
	)
{
	notebookSelection = notebook.name;
	copy(this->notes.begin(), this->notes.end(), back_inserter(notes));
}

void MockUserModel::GetNotesBySearch
	( const std::wstring & search
	, NoteList           & notes
	)
{
	searchSelection = search;
	copy(this->notes.begin(), this->notes.end(), back_inserter(notes));
}

void MockUserModel::GetResource(const Guid & guid, Resource & resource)
{
	ResourceMap::iterator i(resources.find(guid));
	if (i != resources.end())
		resource = i->second;
}

void MockUserModel::GetTags(TagList & tags)
{
	copy(this->tags.begin(), this->tags.end(), back_inserter(tags));
}

void MockUserModel::Load(const wstring & username)
{
	if (loadMethod != LoadMethodNone)
	{
		loadMethod = LoadMethodNone;
		return;
	}
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

void MockUserModel::SetCredentials
		( const wstring & username
		, const wstring & password
		)
{
	credentials.SetUsername(username);
	credentials.SetPassword(password);
}

void MockUserModel::SetNoteThumbnail(const Guid & guid, const Thumbnail & thumbnail)
{
	noteThumbnails[guid] = thumbnail;
}

void MockUserModel::Unload()
{
	loadMethod = LoadMethodNone;
	loadedAs.clear();
}
