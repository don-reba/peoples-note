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
	notebooks.resize(1);
	notebooks.at(0).SetName(L"Notes");
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

void UserModel::Load()
{
	// TODO: implement
	SignalLoaded();
}
