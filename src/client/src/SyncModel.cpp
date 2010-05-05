#include "stdafx.h"
#include "SyncModel.h"

#include "DataStore.h"
#include "IEnService.h"
#include "IMessagePump.h"
#include "INoteStore.h"
#include "IUserStore.h"
#include "NotebookProcessor.h"
#include "NoteProcessor.h"
#include "ScopedLock.h"
#include "SyncLogic.h"
#include "TagProcessor.h"
#include "Tools.h"
#include "IUserModel.h"

using namespace std;

//----------
// interface
//----------

SyncModel::SyncModel
	( IEnService   & enService
	, IMessagePump & messagePump
	)
	: syncThread  (NULL)
	, enService   (enService)
	, messagePump (messagePump)
{
	::InitializeCriticalSection(&lock);
}

SyncModel::~SyncModel()
{
	CloseThread();
}

void SyncModel::ProcessMessages()
{
	ScopedLock lock(lock);
	while (!messages.empty())
	{
		switch (messages.front())
		{
		case MessageSyncComplete:
			SignalSyncComplete();
			break;
		default:
			break;
		}
		messages.pop();
	}
}

void SyncModel::StopSync()
{
	if (syncContext)
		syncContext->SetStopRequested(true);
	if (syncThread)
		::WaitForSingleObject(syncThread, INFINITE);
}

//--------------------------
// ISyncModel implementation
//--------------------------

void SyncModel::BeginSync(IUserModel & userModel)
{
	CloseThread();
	syncContext.reset(new SyncContext(*this, userModel));
	syncThread = ::CreateThread
		( NULL             // lpsa
		, 0                // cbStack
		, &SyncModel::Sync // lpStartAddr
		, &*syncContext    // lpvThreadParam
		, 0                // fdwCreate
		, NULL             // lpIDThread
		);
}

void SyncModel::ConnectSyncComplete(slot_type OnSyncComplete)
{
	SignalSyncComplete.connect(OnSyncComplete);
}

//------------------
// utility functions
//------------------

void SyncModel::CloseThread()
{
	if (!syncThread)
		return;
	StopSync();
	::CloseHandle(syncThread);
	syncThread = NULL;
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	SyncContext * context(reinterpret_cast<SyncContext*>(param));
	try
	{
		IUserModel & userModel (context->GetUserModel());
		IEnService & enService (context->GetEnService());

		IEnService::UserStore userStore(enService.GetUserStore());
		Credentials credentials(userModel.GetCredentials());
		IUserStore::AuthenticationResult authenticationResult
			( userStore->GetAuthenticationToken
				( credentials.GetUsername()
				, credentials.GetPassword()
				)
			);
		if (!authenticationResult.IsGood)
		{
			context->EnqueueMessage(MessageSyncFailed);
			return 0;
		}

		IEnService::NoteStore noteStore
			( enService.GetNoteStore(authenticationResult.Token)
			);

		Notebook notebook(userModel.GetLastUsedNotebook());

		EnInteropNoteList remoteNotes;
		NotebookList      remoteNotebooks;
		TagList           remoteTags;
		noteStore->ListEntries(remoteNotes, remoteNotebooks, remoteTags);

		SyncLogic syncLogic;

		const NotebookList & localNotebooks(userModel.GetNotebooks());
		syncLogic.FullSync
			( remoteNotebooks
			, localNotebooks
			, NotebookProcessor(*noteStore, userModel)
			);

		const TagList & localTags(userModel.GetTags());
		syncLogic.FullSync
			( remoteTags
			, localTags
			, TagProcessor(*noteStore, userModel)
			);

		const EnInteropNoteList localNotes; // TODO: initialize
		syncLogic.FullSync
			( remoteNotes
			, localNotes
			, NoteProcessor(*noteStore, userModel, notebook)
			);
	}
	catch (const std::exception &)
	{
		context->EnqueueMessage(MessageSyncFailed);
		return 0;
	}
	context->EnqueueMessage(MessageSyncComplete);
	return 0;
}

//---------------------------
// SyncContext implementation
//---------------------------

SyncModel::SyncContext::SyncContext
	( SyncModel  & syncModel
	, IUserModel & userModel
	)
	: syncModel (syncModel)
	, userModel (userModel)
{
}

void SyncModel::SyncContext::EnqueueMessage(Message message)
{
	ScopedLock lock(syncModel.lock);
	syncModel.messages.push(message);
	syncModel.messagePump.WakeUp();
}

IEnService & SyncModel::SyncContext::GetEnService()
{
	return syncModel.enService;
}

bool SyncModel::SyncContext::GetStopRequested() const
{
	bool stopRequested;
	{
		ScopedLock lock(syncModel.lock);
		stopRequested = this->stopRequested;
	}
	return stopRequested;
}

void SyncModel::SyncContext::SetStopRequested(bool value)
{
	ScopedLock lock(syncModel.lock);
	stopRequested = value;
}

IUserModel & SyncModel::SyncContext::GetUserModel()
{
	return userModel;
}
