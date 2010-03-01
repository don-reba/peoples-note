#include "stdafx.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

MockUserModel::MockUserModel()
	: isDefault (false)
	, isLoaded  (false)
{
}

void MockUserModel::Loaded()
{
	SignalLoaded();
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

INotebook & MockUserModel::GetLastUsedNotebook()
{
	return lastUsedNotebook;
}

ptr_vector<INotebook> & MockUserModel::GetNotebooks()
{
	return notebooks;
}

ptr_vector<INote> & MockUserModel::GetNotesByNotebook(INotebook & notebook)
{
	notebookSelection = notebook.GetName();
	return notes;
}

ptr_vector<INote> & MockUserModel::GetNotesBySearch(wstring search)
{
	searchSelection = search;
	return notes;
}

void MockUserModel::Load()
{
	isLoaded = true;
}
