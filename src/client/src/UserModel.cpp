#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
#include "Notebook.h"

#include <algorithm>
#include <iterator>

using namespace boost;
using namespace std;

UserModel::UserModel(IDataStore & dataStore)
	: dataStore        (dataStore)
	, lastUsedNotebook (L"temp")
{
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

INotebook & UserModel::GetLastUsedNotebook()
{
	return dataStore.GetLastUsedNotebook();
}

ptr_vector<INotebook> & UserModel::GetNotebooks()
{
	return dataStore.GetNotebooks();
}

ptr_vector<INote> & UserModel::GetNotesByNotebook(INotebook & notebook)
{
	return dataStore.GetNotesByNotebook(notebook);
}

ptr_vector<INote> & UserModel::GetNotesBySearch(wstring search)
{
	return dataStore.GetNotesBySearch(search);
}

void UserModel::Load()
{
	// TODO: handle failure
	dataStore.LoadOrCreate(credentialsModel.GetUsername());
	if (dataStore.GetNotebookCount() == 0)
	{
		Notebook defaultNotebook(L"Notes");
		dataStore.AddNotebook(defaultNotebook);
		dataStore.MakeNotebookDefault(defaultNotebook);
	}
	SignalLoaded();
}
