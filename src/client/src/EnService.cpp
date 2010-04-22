#include "stdafx.h"
#include "EnService.h"

#include "IUserModel.h"

#include "Thrift/Thrift.h"
#include "Evernote/EDAM/NoteStore.h"

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

		THttpTransport  noteStoreTransport ((noteStoreUrlBase + authenticationResult.user.shardId).c_str());
		TBinaryProtocol noteStoreProtocol  (noteStoreTransport);
		noteStoreTransport.Open();

		NoteStore::NoteStore::Client noteStore(noteStoreProtocol);

		NoteStore::SyncChunk chunk = noteStore.getSyncChunk
			( authenticationResult.authenticationToken
			, 0
			, 10
			, true
			);

		Notebook lastUsedNotebook(userModel.GetLastUsedNotebook());
		foreach (const Types::Note & note, chunk.notes)
		{
			Note x(note.guid, note.title, (time_t)note.created);
			userModel.AddNote(x, L"", L"", lastUsedNotebook);
		}

		foreach (const Types::Notebook & notebook, chunk.notebooks)
		{
			Notebook x(notebook.guid, notebook.name);
			userModel.AddNotebook(x);
		}
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
