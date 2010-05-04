#include "stdafx.h"
#include "NoteProcessor.h"

#include "INoteStore.h"
#include "IUserModel.h"
#include "Note.h"
#include "Notebook.h"
#include "Resource.h"
#include "Tools.h"
#include "Transaction.h"

using namespace std;
using namespace Tools;

NoteProcessor::NoteProcessor
	( INoteStore & noteStore
	, IUserModel & userModel
	, Notebook   & notebook
	)
	: noteStore (noteStore)
	, userModel (userModel)
	, notebook  (notebook)
{
}

void NoteProcessor::Add(const EnInteropNote & remote)
{
	wstring body;
	noteStore.GetNoteBody(remote.GetNote(), body);

	Transaction transaction(userModel);
	userModel.AddNote(remote.GetNote(), body, L"", notebook);
	foreach (const Guid & guid, remote.GetResources())
	{
		Resource resource;
		noteStore.GetNoteResource
			( guid
			, resource
			);
		userModel.AddImageResource
			( HashWithMD5(resource.Data)
			, resource.Data
			, remote.GetNote().GetGuid()
			);
	}
}

void NoteProcessor::Delete(const EnInteropNote & local)
{
	userModel.DeleteNote(local.GetNote());
}

void NoteProcessor::Rename(const EnInteropNote &)
{
}

void NoteProcessor::Upload(const EnInteropNote & local)
{
	vector<Resource> resources;
	userModel.GetNoteResources(local.GetNote(), resources);
	noteStore.CreateNote(local.GetNote(), resources);
}

void NoteProcessor::Merge
	( const EnInteropNote & local
	, const EnInteropNote & remote
	)
{
}
