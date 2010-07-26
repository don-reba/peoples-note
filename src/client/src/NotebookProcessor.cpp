#include "stdafx.h"
#include "NotebookProcessor.h"

#include "INoteStore.h"
#include "IUserModel.h"
#include "Notebook.h"
#include "Transaction.h"

#include <algorithm>
#include <sstream>

using namespace std;

NotebookProcessor::NotebookProcessor(IUserModel & userModel)
	: userModel (userModel)
{
}

void NotebookProcessor::Add(const Notebook & remote)
{
	userModel.AddNotebook(remote);
}

void NotebookProcessor::Create
	( const Notebook & local
	, INoteStore     & noteStore
	)
{
	Transaction transaction(userModel);

	Notebook replacement;
	noteStore.CreateNotebook(local, replacement);
	userModel.DeleteNotebook(local.guid);
	userModel.AddNotebook(replacement);
}

void NotebookProcessor::Delete(const Notebook & local)
{
	userModel.DeleteNotebook(local.guid);
}

void NotebookProcessor::Merge
	( const Notebook & local
	, const Notebook & remote
	)
{
	userModel.UpdateNotebook(local, remote);
}

void NotebookProcessor::RenameAdd
	( const Notebook & local
	, const Notebook & remote
	)
{
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);

	vector<wstring> names;
	names.reserve(notebooks.size());
	foreach (const Notebook & notebook, notebooks)
		names.push_back(notebook.name);
	sort(names.begin(), names.end());

	int n(2);
	wstringstream name;
	do
	{
		name.str(wstring());
		name << local.name << L'(' << n << L')';
		++n;
	}
	while (binary_search(names.begin(), names.end(), name.str()));

	Notebook notebook(local);
	notebook.name    = name.str();
	notebook.isDirty = false;
	userModel.UpdateNotebook(local, notebook);
	userModel.AddNotebook(remote);
}

void NotebookProcessor::Update
	( const Notebook & local
	, INoteStore     & noteStore
	)
{
	Transaction transaction(userModel);

	Notebook replacement;
	noteStore.UpdateNotebook(local, replacement);
	userModel.UpdateNotebook(local, replacement);
}
