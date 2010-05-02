#include "stdafx.h"
#include "NoteProcessor.h"

#include "IEnService.h"
#include "IUserModel.h"
#include "Notebook.h"

NoteProcessor::NoteProcessor
	( IEnService & enService
	, IUserModel & userModel
	, Notebook   & notebook
	)
	: enService (enService)
	, userModel (userModel)
	, notebook  (notebook)
{
}

void NoteProcessor::Add(const Note & remote)
{
}

void NoteProcessor::Delete(const Note & local)
{
}

void NoteProcessor::Rename(const Note & local)
{
}

void NoteProcessor::Upload(const Note & local)
{
}

void NoteProcessor::Merge
	( const Note & local
	, const Note & remote
	)
{
}
