#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
#include "Notebook.h"

#include <algorithm>
#include <iterator>

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
	// TODO: implement
	return lastUsedNotebook;
}

vector<INotebook*> UserModel::GetNotebooks() const
{
	// TODO: implement
	return vector<INotebook*>();
}

vector<INote*> UserModel::GetNotesByNotebook(INotebook & notebook) const
{
	// TODO: implement
	return vector<INote*>();
}

vector<INote*> UserModel::GetNotesBySearch(wstring search) const
{
	// TODO: implement
	return vector<INote*>();
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
