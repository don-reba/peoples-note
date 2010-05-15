#include "stdafx.h"
#include "SyncModel.h"

#include "DataStore.h"
#include "IEnService.h"
#include "IMessagePump.h"
#include "INoteStore.h"
#include "ISyncLogger.h"
#include "IUserStore.h"
#include "NotebookProcessor.h"
#include "NoteProcessor.h"
#include "ScopedLock.h"
#include "SyncLogic.h"
#include "TagProcessor.h"
#include "Tools.h"
#include "IUserModel.h"

using namespace std;
using namespace Tools;

//----------
// interface
//----------

SyncModel::SyncModel
	( IEnService   & enService
	, IMessagePump & messagePump
	, IUserModel   & userModel
	, ISyncLogger  & logger
	)
	: syncThread  (NULL)
	, enService   (enService)
	, messagePump (messagePump)
	, userModel   (userModel)
	, syncLogger  (logger)
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
	while (!messages.IsEmpty())
	{
		switch (messages.Dequeue())
		{
		case SyncMessageQueue::MessageNotebooksChanged:
			SignalNotebooksChanged();
			break;
		case SyncMessageQueue::MessageNotesChanged:
			SignalNotesChanged();
			break;
		case SyncMessageQueue::MessageTagsChanged:
			SignalTagsChanged();
			break;
		case SyncMessageQueue::MessageSyncComplete:
			SignalSyncComplete();
			break;
		default:
			break;
		}
	}
}

void SyncModel::StopSync()
{
	ScopedLock lock(lock);
	stopRequested = true;
	if (syncThread)
		::WaitForSingleObject(syncThread, INFINITE);
}

//--------------------------
// ISyncModel implementation
//--------------------------

void SyncModel::BeginSync(const wstring & username)
{
	CloseThread();
	userModel.Load(username);
	syncThread = ::CreateThread
		( NULL             // lpsa
		, 0                // cbStack
		, &SyncModel::Sync // lpStartAddr
		, this             // lpvThreadParam
		, 0                // fdwCreate
		, NULL             // lpIDThread
		);
}

void SyncModel::ConnectNotebooksChanged(slot_type OnNotebooksChanged)
{
	SignalNotebooksChanged.connect(OnNotebooksChanged);
}

void SyncModel::ConnectNotesChanged(slot_type OnNotesChanged)
{
	SignalNotesChanged.connect(OnNotesChanged);
}

void SyncModel::ConnectSyncComplete(slot_type OnSyncComplete)
{
	SignalSyncComplete.connect(OnSyncComplete);
}

void SyncModel::ConnectTagsChanged(slot_type OnTagsChanged)
{
	SignalTagsChanged.connect(OnTagsChanged);
}

//------------------
// utility functions
//------------------

void SyncModel::CloseThread()
{
	StopSync();
	::CloseHandle(syncThread);
	syncThread = NULL;
}

void SyncModel::PostMessage(SyncMessageQueue::Message message)
{
	messages.Enqueue(message);
	messagePump.WakeUp();
}

void SyncModel::Sync()
{
	try
	{
		IEnService::UserStorePtr userStore(enService.GetUserStore());
		Credentials credentials;
		userModel.GetCredentials(credentials);
		IUserStore::AuthenticationResult authenticationResult
			( userStore->GetAuthenticationToken
				( credentials.GetUsername()
				, credentials.GetPassword()
				)
			);
		if (!authenticationResult.IsGood)
		{
			PostMessage(SyncMessageQueue::MessageSyncFailed);
			return;
		}

		IEnService::NoteStorePtr noteStore
			( enService.GetNoteStore
				( authenticationResult.Token
				, authenticationResult.ShardId
				)
			);

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);

		EnInteropNoteList remoteNotes;
		NotebookList      remoteNotebooks;
		TagList           remoteTags;
		noteStore->ListEntries(remoteNotes, remoteNotebooks, remoteTags);

		syncLogger.ListRemoteNotes     (remoteNotes);
		syncLogger.ListRemoteNotebooks (remoteNotebooks);
		syncLogger.ListRemoteTags      (remoteTags);

		SyncLogic syncLogic(syncLogger);

		NotebookList localNotebooks;
		userModel.GetNotebooks(localNotebooks);
		syncLogger.ListLocalNotebooks(localNotebooks);

		syncLogger.BeginSyncStage(L"notebooks");
		syncLogic.FullSync
			( remoteNotebooks
			, localNotebooks
			, NotebookProcessor(*noteStore, userModel)
			);

		TagList localTags;
		userModel.GetTags(localTags);
		syncLogger.ListLocalTags(localTags);

		syncLogger.BeginSyncStage(L"tags");
		syncLogic.FullSync
			( remoteTags
			, localTags
			, TagProcessor(*noteStore, userModel)
			);

		const EnInteropNoteList localNotes; // TODO: initialize
		syncLogger.ListLocalNotes(localNotes);

		syncLogger.BeginSyncStage(L"notes");
		syncLogic.FullSync
			( remoteNotes
			, localNotes
			, NoteProcessor(*noteStore, userModel, notebook)
			);

		PostMessage(SyncMessageQueue::MessageSyncComplete);

		userModel.Unload();
	}
	catch (const std::exception & e)
	{
		syncLogger.Error(ConvertToUnicode(e.what()));
		PostMessage(SyncMessageQueue::MessageSyncFailed);
		userModel.Unload();
	}
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	reinterpret_cast<SyncModel*>(param)->Sync();
	return 0;
}
