#include "stdafx.h"
#include "NoteProcessor.h"

#include "EnInteropNote.h"
#include "INoteStore.h"
#include "IUserModel.h"
#include "Notebook.h"
#include "Transaction.h"

using namespace std;

NoteProcessor::NoteProcessor
	( IUserModel     & userModel
	, INoteStore     & noteStore
	, const Notebook & notebook
	)
	: userModel (userModel)
	, noteStore (noteStore)
	, notebook  (notebook)
{
}

void NoteProcessor::Add(const EnInteropNote & remote)
{
	wstring body;
	noteStore.GetNoteBody(remote.note, body);

	vector<wstring> tagNames;
	noteStore.GetNoteTagNames(remote.note, tagNames);

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
	foreach (const wstring & tagName, tagNames)
		userModel.AddTagToNote(tagName, remote.note);
}

void NoteProcessor::Delete(const EnInteropNote & local)
{
	userModel.DeleteNote(local.note.guid);
}

void NoteProcessor::Create(const EnInteropNote & local)
{
	Transaction transaction(userModel);

	vector<Resource> resources(local.resources.size());
	for (int i(0); i != resources.size(); ++i)
		userModel.GetResource(local.resources[i], resources[i]);

	wstring body;
	userModel.GetNoteBody(local.guid, body);

	Note replacement;
	noteStore.CreateNote(local.note, body, resources, notebook.guid, replacement);

	userModel.DeleteNote(local.note.guid);
	userModel.AddNote(replacement, body, L"", notebook);
}

void NoteProcessor::Merge
	( const EnInteropNote & local
	, const EnInteropNote & remote
	)
{
	Transaction transaction(userModel);

	userModel.DeleteNote(local.note.guid);

	wstring body;
	noteStore.GetNoteBody(remote.note, body);

	userModel.AddNote(remote.note, body, L"", notebook);

	foreach (const Guid & guid, remote.resources)
	{
		Resource resource;
		noteStore.GetNoteResource(guid, resource);
		userModel.AddResource(resource);
	}
}

void NoteProcessor::RenameAdd
	( const EnInteropNote & local
	, const EnInteropNote & remote
	)
{
	// note names need not be unique
	Add(remote);
}

void NoteProcessor::Update(const EnInteropNote & local)
{
	Transaction transaction(userModel);

	vector<Resource> resources(local.resources.size());
	for (int i(0); i != resources.size(); ++i)
		userModel.GetResource(local.resources[i], resources[i]);

	wstring body;
	userModel.GetNoteBody(local.guid, body);

	Note replacement;
	noteStore.UpdateNote(local.note, body, resources, notebook.guid, replacement);

	userModel.DeleteNote(local.note.guid);
	userModel.AddNote(replacement, body, L"", notebook);
}
