#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
#include "Notebook.h"

#include <algorithm>
#include <iterator>

using namespace boost;
using namespace std;

UserModel::UserModel(IDataStore & dataStore)
	: dataStore (dataStore)
{
}

void UserModel::AddNote(const Note & note, const Notebook & notebook)
{
	dataStore.AddNote(note, notebook);
}

void UserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void UserModel::CreateDefaultUser()
{
	credentialsModel.SetCredentials(L"", L"");
}

void UserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}

Notebook & UserModel::GetLastUsedNotebook()
{
	return dataStore.GetLastUsedNotebook();
}

const NotebookList & UserModel::GetNotebooks()
{
	return dataStore.GetNotebooks();
}

const NoteList & UserModel::GetNotesByNotebook(const Notebook & notebook)
{
	return dataStore.GetNotesByNotebook(notebook);
}

const NoteList & UserModel::GetNotesBySearch(wstring search)
{
	return dataStore.GetNotesBySearch(search);
}

void UserModel::Load()
{
	// TODO: handle failure
	dataStore.LoadOrCreate(credentialsModel.GetUsername());
	if (dataStore.GetNotebookCount() == 0)
	{
		Notebook defaultNotebook(Guid(), L"Notes");
		dataStore.AddNotebook(defaultNotebook);
		dataStore.MakeNotebookDefault(defaultNotebook);
		dataStore.MakeNotebookLastUsed(defaultNotebook);
	}
	SignalLoaded();
}
