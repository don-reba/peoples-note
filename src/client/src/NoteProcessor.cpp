#include "stdafx.h"
#include "NoteProcessor.h"

#include "EnInteropNote.h"
#include "INoteStore.h"
#include "Notebook.h"
#include "Transaction.h"
#include "IUserModel.h"

using namespace std;

NoteProcessor::NoteProcessor(IUserModel & userModel)
	: userModel (userModel)
{
}

void NoteProcessor::Add
	( const EnInteropNote & remote
	, INoteStore          & noteStore
	, Notebook            & notebook
	)
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

void NoteProcessor::RenameAdd
	( const EnInteropNote & local
	, const EnInteropNote & remote
	, INoteStore          & noteStore
	, Notebook            & notebook
	)
{
	// note names need not be unique
	Add(remote, noteStore, notebook);
}

void NoteProcessor::Upload
	( const EnInteropNote & local
	, INoteStore          & noteStore
	, Notebook            & notebook
	)
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
