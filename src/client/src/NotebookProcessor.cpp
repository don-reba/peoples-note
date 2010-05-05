#include "stdafx.h"
#include "NotebookProcessor.h"

#include "INoteStore.h"
#include "IUserModel.h"

#include <algorithm>
#include <sstream>

using namespace std;

NotebookProcessor::NotebookProcessor
	( INoteStore & noteStore
	, IUserModel & userModel
	)
	: noteStore (noteStore)
	, userModel (userModel)
{
}

void NotebookProcessor::Add(const Notebook & remote)
{
	userModel.AddNotebook(remote);
}

void NotebookProcessor::Delete(const Notebook & local)
{
	userModel.DeleteNotebook(local);
}

void NotebookProcessor::Rename(const Notebook & local)
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

	Notebook notebook = local;
	notebook.name = name.str();
	userModel.AddNotebook(notebook);
}

void NotebookProcessor::Upload(const Notebook & local)
{
	noteStore.CreateNotebook(local);
}

void NotebookProcessor::Merge
	( const Notebook & local
	, const Notebook & remote
	)
{
	// keep local
}
