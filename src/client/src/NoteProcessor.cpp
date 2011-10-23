#include "stdafx.h"
#include "NoteProcessor.h"

#include "EnInteropNote.h"
#include "EnNoteTranslator.h"
#include "INoteStore.h"
#include "IUserModel.h"
#include "Notebook.h"
#include "Transaction.h"

using namespace std;

NoteProcessor::NoteProcessor
	( EnNoteTranslator & enNoteTranslator
	, IUserModel       & userModel
	, INoteStore       & noteStore
	, const Notebook   & notebook
	)
	: enNoteTranslator (enNoteTranslator)
	, userModel        (userModel)
	, noteStore        (noteStore)
	, notebook         (notebook)
{
}

void NoteProcessor::AddLocal(const EnInteropNote & remote)
{
	wstring body;
	noteStore.GetNoteBody(remote.note, body);

	wstring bodyText;
	enNoteTranslator.ConvertToText(body, bodyText);

	Transaction transaction(userModel);
	userModel.ReplaceNote(remote.note, body, bodyText, notebook);
	foreach (const Guid & guid, remote.resources)
	{
		Resource             resource;
		RecognitionEntryList recognitionEntries;

		noteStore.GetNoteResource(guid, resource, recognitionEntries);
		userModel.AddResource(resource);
		foreach (const RecognitionEntry & entry, recognitionEntries)
			userModel.AddRecognitionEntry(entry);
	}
	foreach (const Guid & tag, remote.tags)
		userModel.AddTagToNote(tag, remote.note);
}

void NoteProcessor::DeleteLocal(const EnInteropNote & local)
{
	userModel.ExpungeNote(local.note.guid);
}

void NoteProcessor::DeleteRemote(const Guid & guid)
{
	noteStore.DeleteNote(guid);
}

void NoteProcessor::CreateRemote(const EnInteropNote & local)
{
	Transaction transaction(userModel);

	vector<Resource> resources(local.resources.size());
	for (int i(0); i != resources.size(); ++i)
		userModel.GetResource(local.resources[i], resources[i]);

	wstring body;
	userModel.GetNoteBody(local.guid, body);

	wstring bodyText;
	enNoteTranslator.ConvertToText(body, bodyText);

	Note     replacement;
	GuidList replacementTags;
	noteStore.CreateNote
		( local.note
		, body
		, resources
		, notebook.guid
		, replacement
		, replacementTags
		);

	userModel.ExpungeNote(local.guid);
	userModel.AddNote(replacement, body, bodyText, notebook);
	
	foreach (const Guid & tag, replacementTags)
		userModel.AddTagToNote(tag, replacement);

	foreach (Resource & resource, resources)
	{
		resource.Note = replacement.guid;
		userModel.AddResource(resource);
	}
}

void NoteProcessor::MergeLocal
	( const EnInteropNote &
	, const EnInteropNote & remote
	)
{
	AddLocal(remote);
}

void NoteProcessor::UpdateRemote(const EnInteropNote & local)
{
	Transaction transaction(userModel);

	vector<Resource> resources(local.resources.size());
	for (int i(0); i != resources.size(); ++i)
		userModel.GetResource(local.resources[i], resources[i]);

	wstring body;
	userModel.GetNoteBody(local.guid, body);

	wstring bodyText;
	enNoteTranslator.ConvertToText(body, bodyText);

	TagList tags;
	userModel.GetNoteTags(local.note, tags);
	GuidList tagGuids;
	tagGuids.reserve(tags.size());
	foreach (const Tag & tag, tags)
		tagGuids.push_back(tag.guid);

	Note     replacement;
	GuidList replacementTags;
	noteStore.UpdateNote
		( local.note
		, body
		, tagGuids
		, resources
		, notebook.guid
		, replacement
		, replacementTags
		);

	userModel.ExpungeNote(local.note.guid);
	userModel.AddNote(replacement, body, bodyText, notebook);

	foreach (const Guid & tag, replacementTags)
		userModel.AddTagToNote(tag, replacement);

	foreach (Resource & resource, resources)
	{
		resource.Note = replacement.guid;
		userModel.AddResource(resource);
	}
}
