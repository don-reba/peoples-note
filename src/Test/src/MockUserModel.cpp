#include "stdafx.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isDefault        (false)
	, isLoaded         (false)
	, lastUsedNotebook (Guid(), L"default-notebook")
{
}

void MockUserModel::Loaded()
{
	SignalLoaded();
}

void MockUserModel::AddNote
	( const Note     & note
	, const Notebook & notebook
	)
{
	addedNotes.push_back(make_pair(note.GetTitle(), notebook.GetName()));
	notes.push_back(note);
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void MockUserModel::CreateDefaultUser()
{
	isDefault = true;
}

void MockUserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}

Notebook & MockUserModel::GetLastUsedNotebook()
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
