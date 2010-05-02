#include "stdafx.h"
#include "NotebookProcessor.h"

#include "IEnService.h"
#include "IUserModel.h"

NotebookProcessor::NotebookProcessor
	( IEnService & enService
	, IUserModel & userModel
	)
	: enService (enService)
	, userModel (userModel)
{
}

void NotebookProcessor::Add(const Notebook & remote)
{
}

void NotebookProcessor::Delete(const Notebook & local)
{
}

void NotebookProcessor::Rename(const Notebook & local)
{
}

void NotebookProcessor::Upload(const Notebook & local)
{
}

void NotebookProcessor::Merge
	( const Notebook & local
	, const Notebook & remote
	)
{
}
