#include "stdafx.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: defaultNotebook  (Guid(), L"default-notebook")
	, lastUsedNotebook (Guid(), L"last-used-notebook")
	, loadMethod       (LoadMethodNone)
{
}

void MockUserModel::AddImageResource
	( string       hash
	, const Blob & data
	, Guid         note
	)
{
	addedImages.push_back(ImageRecord(hash, data, note));
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
	noteBodies[note.GetGuid()] = body;
}

void MockUserModel::AddNotebook(const Notebook & notebook)
{
	notebooks.push_back(notebook);
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

bool MockUserModel::Exists(const wstring & username)
{
	return validUsernames.find(username) != validUsernames.end();
}

Credentials MockUserModel::GetCredentials()
{
	return credentials;
}

Notebook MockUserModel::GetDefaultNotebook()
{
	return defaultNotebook;
}

Notebook MockUserModel::GetLastUsedNotebook()
{
	return lastUsedNotebook;
}

void MockUserModel::GetImageResource(string hash, Blob & blob)
{
	if (images.find(hash) != images.end())
		blob = images[hash];
}

Note MockUserModel::GetNote(Guid guid)
{
	const string & guidString = static_cast<const string &>(guid);
	foreach (Note & note, notes)
	{
		if (guidString == static_cast<const string &>(note.GetGuid()))
			return note;
	}
	throw std::exception("Note not found.");
}

void MockUserModel::GetNoteBody(Guid guid, wstring & resource)
{
	if (noteBodies.find(guid) != noteBodies.end())
		resource = noteBodies[guid];
}

void MockUserModel::GetNoteThumbnail(const Guid & guid, Thumbnail & thumbnail)
{
	if (noteThumbnails.end() != noteThumbnails.find(guid))
		thumbnail = noteThumbnails[guid];
}

const NotebookList & MockUserModel::GetNotebooks()
{
	return notebooks;
}

const NoteList & MockUserModel::GetNotesByNotebook(const Notebook & notebook)
{
	notebookSelection = notebook.GetName();
	return notes;
}

const NoteList & MockUserModel::GetNotesBySearch(wstring search)
{
	searchSelection = search;
	return notes;
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
