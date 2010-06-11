#include "stdafx.h"
#include "SyncModel.h"

#include "DataStore.h"
#include "IEnService.h"
#include "IMessagePump.h"
#include "INoteStore.h"
#include "ISyncLogger.h"
#include "IUserModel.h"
#include "IUserStore.h"
#include "NotebookProcessor.h"
#include "NoteProcessor.h"
#include "ScopedLock.h"
#include "SyncLogic.h"
#include "TagProcessor.h"
#include "Tools.h"
#include "Transaction.h"

#include <algorithm>
#include <sstream>

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

void SyncModel::GetNotes
	( const Notebook    & notebook
	, EnInteropNoteList & notes
	)
{
	NoteList source;
	userModel.GetNotesByNotebook(notebook, source);
	foreach (const Note & note, source)
	{
		notes.push_back(EnInteropNote());
		notes.back().note     = note;
		notes.back().notebook = notebook.guid;
		notes.back().guid    = note.guid;
		notes.back().isDirty = note.isDirty;
		notes.back().name    = note.name;
		notes.back().usn     = note.usn;
	}
}

void SyncModel::PostMessage(SyncMessageQueue::Message message)
{
	messages.Enqueue(message);
	messagePump.WakeUp();
}

void SyncModel::ProcessNotes
	( const EnInteropNoteList & remote
	, INoteStore              & noteStore
	, Notebook                & notebook
	)
{
	EnInteropNoteList local;
	GetNotes(notebook, local);
	syncLogger.ListLocalNotes(local);

	vector<SyncLogic::Action<EnInteropNote> > actions;
	SyncLogic::FullSync(remote, local, actions);

	NoteProcessor processor(userModel, noteStore, notebook);

	syncLogger.BeginSyncStage(L"notes");
	foreach (const SyncLogic::Action<EnInteropNote> action, actions)
	{
		// filter by notes from this notebook
		if (action.Local && action.Local->notebook != notebook.guid)
			continue;
		if (action.Remote && action.Remote->notebook != notebook.guid)
			continue;

		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			processor.Add(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			processor.Delete(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			processor.RenameAdd(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			processor.Upload(*action.Local);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			processor.Merge(*action.Local, *action.Remote);
			break;
		}
	}
}

void SyncModel::ProcessNotebooks
	( const NotebookList & remote
	, INoteStore         & noteStore
	)
{
	NotebookList local;
	userModel.GetNotebooks(local);
	syncLogger.ListLocalNotebooks(local);

	vector<SyncLogic::Action<Notebook> > actions;
	SyncLogic::FullSync(remote, local, actions);

	NotebookProcessor processor(userModel);

	syncLogger.BeginSyncStage(L"notebooks");
	foreach (const SyncLogic::Action<Notebook> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			processor.Add(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			processor.Delete(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			processor.RenameAdd(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			processor.Upload(*action.Local, noteStore);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			processor.Merge(*action.Local, *action.Remote);
			break;
		}
	}
}

void SyncModel::ProcessTags
	( const TagList & remote
	, INoteStore    & noteStore
	)
{
	TagList local;
	userModel.GetTags(local);
	syncLogger.ListLocalTags(local);

	vector<SyncLogic::Action<Tag> > actions;
	SyncLogic::FullSync(remote, local, actions);

	TagProcessor processor(userModel);

	syncLogger.BeginSyncStage(L"tags");
	foreach (const SyncLogic::Action<Tag> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			processor.Add(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			processor.Delete(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			processor.RenameAdd(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			processor.Upload(*action.Local, noteStore);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			processor.Merge(*action.Local, *action.Remote);
			break;
		}
	}
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
			userModel.Unload();
			syncLogger.Error(L"Authentication failed.");
			PostMessage(SyncMessageQueue::MessageSyncFailed);
			return;
		}

		IEnService::NoteStorePtr noteStore
			( enService.GetNoteStore
				( authenticationResult.Token
				, authenticationResult.ShardId
				)
			);

		EnInteropNoteList remoteNotes;
		NotebookList      remoteNotebooks;
		TagList           remoteTags;

		noteStore->ListEntries(remoteNotes, remoteNotebooks, remoteTags);

		syncLogger.ListRemoteNotes     (remoteNotes);
		syncLogger.ListRemoteNotebooks (remoteNotebooks);
		syncLogger.ListRemoteTags      (remoteTags);

		ProcessNotebooks (remoteNotebooks, *noteStore);
		ProcessTags      (remoteTags,      *noteStore);

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);

		ProcessNotes(remoteNotes, *noteStore, notebook);

		PostMessage(SyncMessageQueue::MessageSyncComplete);

		userModel.Unload();
	}
	catch (const std::exception & e)
	{
		syncLogger.Error(ConvertToUnicode(e.what()));
		userModel.Unload();
		PostMessage(SyncMessageQueue::MessageSyncFailed);
	}
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	reinterpret_cast<SyncModel*>(param)->Sync();
	return 0;
}
