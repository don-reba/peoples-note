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
	userModel.UpdateNotebook(local, replacement);
}

void NotebookProcessor::Delete(const Notebook & local)
{
	userModel.ExpungeNotebook(local.guid);
}

void NotebookProcessor::Merge
	( const Notebook & local
	, const Notebook & remote
	)
{
	userModel.UpdateNotebook(local, remote);
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
