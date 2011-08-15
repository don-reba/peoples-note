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

void NotebookProcessor::AddLocal(const Notebook & remote)
{
	userModel.AddNotebook(remote);
}

void NotebookProcessor::CreateRemote
	( const Notebook & local
	, INoteStore     & noteStore
	)
{
	Transaction transaction(userModel);

	Notebook replacement;
	noteStore.CreateNotebook(local,      replacement);
	userModel.UpdateNotebook(local.guid, replacement);
}

void NotebookProcessor::DeleteLocal(const Notebook & local)
{
	userModel.ExpungeNotebook(local.guid);
}

void NotebookProcessor::MergeLocal
	( const Notebook & local
	, const Notebook & remote
	)
{
	userModel.UpdateNotebook(local.guid, remote);
}

void NotebookProcessor::UpdateRemote
	( const Notebook & local
	, INoteStore     & noteStore
	)
{
	Transaction transaction(userModel);

	Notebook replacement;
	noteStore.UpdateNotebook(local,      replacement);
	userModel.UpdateNotebook(local.guid, replacement);
}
