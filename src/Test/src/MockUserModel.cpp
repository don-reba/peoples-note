#include "stdafx.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isDefault        (false)
	, isLoaded         (false)
	, defaultNotebook  (Guid(), L"default-notebook")
	, lastUsedNotebook (Guid(), L"last-used-notebook")
{
}

void MockUserModel::AddNote
	( const Note     & note
	, const Notebook & notebook
	)
{
	addedNotes.push_back(make_pair(note.GetTitle(), notebook.GetName()));
	notes.push_back(note);
}

void MockUserModel::AddNotebook(const Notebook & notebook)
{
	notebooks.push_back(notebook);
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void MockUserModel::CreateDefaultUser()
{
	isDefault = true;
}

Notebook MockUserModel::GetDefaultNotebook()
{
	return defaultNotebook;
}

Notebook MockUserModel::GetLastUsedNotebook()
{
	return lastUsedNotebook;
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

void MockUserModel::Load()
{
	isLoaded = true;
}

void MockUserModel::MakeNotebookDefault(const Notebook & notebook)
{
	defaultNotebook = notebook;
}

void MockUserModel::MakeNotebookLastUsed(const Notebook & notebook)
{
	lastUsedNotebook = notebook;
}

void MockUserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}
