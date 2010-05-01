#include "stdafx.h"/*
#include "EnService.h"

#include "IUserModel.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/NoteStore.h"
#include "SyncLogic.h"
#include "Tools.h"
#include "Transaction.h"

using namespace std;

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

using namespace Evernote::EDAM;

IEnService::CredentialsValidity EnService::CheckCredentials
	( const wstring & username
	, const wstring & password
	)
{
	const wchar_t * consumerKey      = L"donreba";
	const wchar_t * consumerSecret   = L"3d764d03e2b1c7c4";
	const wchar_t * userStoreUrl     = L"https://sandbox.evernote.com/edam/user";
	const wchar_t * noteStoreUrlBase = L"http://sandbox.evernote.com/edam/note/";

	CredentialsValidity validity;

	try
	{
		THttpTransport  transport (userStoreUrl);
		TBinaryProtocol protocol  (transport);
		transport.Open();

		UserStore::UserStore::Client userStore(protocol);

		bool versionOk = userStore.checkVersion
			( L"Peoples' Note"
			, UserStore::constants.EDAM_VERSION_MAJOR
			, UserStore::constants.EDAM_VERSION_MINOR
			);
		if (!versionOk)
		{
			CredentialsValidity validity;
			validity.IsGood  = false;
			validity.Message = L"Incompatible EDAM client protocol version\n";
			return validity;
		}

		userStore.authenticate
			( username.c_str()
			, password.c_str()
			, consumerKey
			, consumerSecret
			);

		validity.IsGood = true;
	}
	catch (const Error::EDAMUserException & e)
	{
		validity.IsGood  = false;
		validity.Message = e.parameter;
	}
	catch (const Error::EDAMSystemException & e)
	{
		validity.IsGood  = false;
		validity.Message = e.message;
	}
	catch (const TException & e)
	{
		validity.IsGood  = false;
		validity.Message = e.GetMessage();
	}
	return validity;
}

class NoteProcessor : public SyncLogic::IResourceProcessor<Note>
{
private:

	IUserModel & userModel;
	NoteStore::NoteStore::Client & noteStore;
	TString & token;

public:

	NoteProcessor
		( IUserModel                   & userModel
		, NoteStore::NoteStore::Client & noteStore
		, TString                      & token
		)
		: userModel (userModel)
		, noteStore (noteStore)
		, token     (token)
	{
	}

	virtual void Add(const Note & remoteNote)
	{
		Notebook notebook(userModel.GetLastUsedNotebook());
		Types::Guid guid = Tools::ConvertToUnicode(remoteNote.GetGuid());
		wstring body = noteStore.getNoteContent(token, guid);
		userModel.AddNote(remoteNote, body, L"", notebook);
	}

	virtual void Delete(const Note & localNote)
	{
		//userModel.DeleteNote(localNote);
	}

	virtual void Rename(const Note & localNote)
	{
	}

	virtual void Upload(const Note & localNote)
	{
		Types::Note note;
		note.__isset.title     = true;
		note.__isset.content   = true;
		note.__isset.resources = true;

		note.title = localNote.GetName();

		userModel.GetNoteBody(localNote.GetGuid(), note.content);

		vector<Blob> resources;
		userModel.GetNoteImageResources(localNote.GetGuid(), resources);
		note.resources.resize(resources.size());
		for (int i(0); i != resources.size(); ++i)
		{
			Types::Resource & resource(note.resources.at(i));
			resource.__isset.data      = true;
			resource.data.__isset.body = true;
			resource.data.body = resources.at(i);
		}

		Types::Note replacement = noteStore.createNote(token, note);
		userModel.AddNote
			( Note
				( replacement.guid
				, replacement.title
				, static_cast<time_t>(replacement.created)
				, replacement.updateSequenceNum
				, false
				)
			, note.content
			, L""
			, userModel.GetLastUsedNotebook()
			);
	}

	virtual void Merge(const Note & localNote, const Note & remoteNote)
	{
	}
};

void EnService::Sync(IUserModel & userModel)
{
	const wchar_t * consumerKey      = L"donreba";
	const wchar_t * consumerSecret   = L"3d764d03e2b1c7c4";
	const wchar_t * userStoreUrl     = L"https://sandbox.evernote.com/edam/user";
	const wchar_t * noteStoreUrlBase = L"http://sandbox.evernote.com/edam/note/";

	try
	{
		THttpTransport  userStoreTransport (userStoreUrl);
		TBinaryProtocol userStoreProtocol  (userStoreTransport);
		userStoreTransport.Open();

		UserStore::UserStore::Client userStore(userStoreProtocol);

		bool versionOk = userStore.checkVersion
			( L"Peoples' Note"
			, UserStore::constants.EDAM_VERSION_MAJOR
			, UserStore::constants.EDAM_VERSION_MINOR
			);
		if (!versionOk)
			return;

		Credentials credentials(userModel.GetCredentials());

		UserStore::AuthenticationResult authenticationResult
			= userStore.authenticate
			( credentials.GetUsername()
			, credentials.GetPassword()
			, consumerKey
			, consumerSecret
			);
		TString token(authenticationResult.authenticationToken);

		THttpTransport  noteStoreTransport ((noteStoreUrlBase + authenticationResult.user.shardId).c_str());
		TBinaryProtocol noteStoreProtocol  (noteStoreTransport);
		noteStoreTransport.Open();

		NoteStore::NoteStore::Client noteStore(noteStoreProtocol);

		NoteStore::SyncChunk chunk = noteStore.getSyncChunk
			( token // authenticationToken
			, 0     // afterUSN
			, 10    // max
			, true  // fullSyncOnly
			);

		Notebook lastUsedNotebook(userModel.GetLastUsedNotebook());

		NoteList remoteNotes;
		foreach (const Types::Note & note, chunk.notes)
		{
			remoteNotes.push_back(Note
				( note.guid
				, note.title
				, static_cast<time_t>(note.created)
				, note.updateSequenceNum
				, false
				));
		}

		const NoteList & localNotes(userModel.GetNotesByNotebook(lastUsedNotebook));

		SyncLogic syncLogic;
		NoteProcessor noteProcessor(userModel, noteStore, token);
		syncLogic.FullSync(remoteNotes, localNotes, noteProcessor);

		//foreach (const Types::Note & note, chunk.notes)
		//{
		//	wstring body = noteStore.getNoteContent(token, note.guid);
		//	Note x(note.guid, note.title, (time_t)note.created);

		//	userModel.AddNote(x, body, L"", lastUsedNotebook);
		//}

		//foreach (const Types::Notebook & notebook, chunk.notebooks)
		//{
		//	Notebook x(notebook.guid, notebook.name);
		//	userModel.AddNotebook(x);
		//}

		// upload

		//const NoteList & localNotes(userModel.GetNotesByNotebook(lastUsedNotebook));
		//foreach (const Note & localNote, localNotes)
		//{
		//	Types::Note note;
		//	note.__isset.title     = true;
		//	note.__isset.content   = true;
		//	note.__isset.resources = true;

		//	note.title = localNote.GetName();

		//	userModel.GetNoteBody(localNote.GetGuid(), note.content);

		//	vector<Blob> resources;
		//	userModel.GetNoteImageResources(localNote.GetGuid(), resources);
		//	note.resources.resize(resources.size());
		//	for (int i(0); i != resources.size(); ++i)
		//	{
		//		Types::Resource & resource(note.resources.at(i));
		//		resource.__isset.data      = true;
		//		resource.data.__isset.body = true;
		//		resource.data.body = resources.at(i);
		//	}

		//	Types::Note replacement = noteStore.createNote(token, note);
		//}
	}
	catch (const Error::EDAMUserException & e)
	{
		DEBUGMSG(true, (L"EDAMUserException: %s\n", e.parameter.c_str()));
	}
	catch (const Error::EDAMSystemException & e)
	{
		DEBUGMSG(true, (L"EDAMSystemException: %s\n", e.message.c_str()));
	}
	catch (const TException & e)
	{
		DEBUGMSG(true, (L"TException: %s\n", e.GetMessage()));
	}
}
*/