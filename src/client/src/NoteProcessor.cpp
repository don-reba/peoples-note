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
	noteStore.GetNoteBody(remote.note, body);

	Transaction transaction(userModel);
	userModel.AddNote(remote.note, body, L"", notebook);
	foreach (const Guid & guid, remote.resources)
	{
		Resource resource;
		noteStore.GetNoteResource(guid, resource);
		// TODO: check the hash here
		// handle incomplete downloads somehow
		// string checkHash = HashWithMD5(resource.Data);
		userModel.AddResource(resource);
	}
}

void NoteProcessor::Delete(const EnInteropNote & local)
{
	userModel.DeleteNote(local.note);
}

void NoteProcessor::Rename(const EnInteropNote &)
{
	// note names need not be unique
}

void NoteProcessor::Upload(const EnInteropNote & local)
{
	Transaction transaction(userModel);

	vector<Resource> resources(local.resources.size());
	for (int i(0); i != resources.size(); ++i)
		userModel.GetResource(local.resources[i], resources[i]);

	wstring body;
	userModel.GetNoteBody(local.guid, body);

	Note replacement;
	noteStore.CreateNote(local.note, body, resources, replacement);

	userModel.DeleteNote(local.note);
	userModel.AddNote(replacement, body, L"", notebook);
}

void NoteProcessor::Merge
	( const EnInteropNote & local
	, const EnInteropNote & remote
	)
{
	// keep local
}
