#include "stdafx.h"
#include "UserModel.h"

#include <algorithm>
#include <iterator>

using namespace std;

void UserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void UserModel::CreateDefaultUser()
{
	credentialsModel.SetCredentials(L"", L"");

	notebooks.clear();
	notebooks.push_back(Notebook(L"Notes"));
}

void UserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}

INotebook & UserModel::GetLastUsedNotebook()
{
	// TODO: implement
	return notebooks.at(0);
}

vector<INotebook*> UserModel::GetNotebooks() const
{
	vector<INotebook*> notebooks;
	notebooks.reserve(this->notebooks.size());
	foreach (Notebook & notebook, this->notebooks)
		notebooks.push_back(&notebook);
	return notebooks;
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
	// TODO: implement
	SignalLoaded();
}
