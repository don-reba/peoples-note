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
	, bool                      fullSync
	)
{
	EnInteropNoteList local;
	GetNotes(notebook, local);
	syncLogger.ListNotes(L"Local notes", local);

	vector<SyncLogic::Action<EnInteropNote> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);
	if (actions.empty())
		return;

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
	, bool                 fullSync
	)
{
	NotebookList local;
	userModel.GetNotebooks(local);
	syncLogger.ListNotebooks(L"Local notebooks", local);

	vector<SyncLogic::Action<Notebook> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);
	if (actions.empty())
		return;

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
	, bool            fullSync
	)
{
	TagList local;
	userModel.GetTags(local);
	syncLogger.ListTags(L"Local tags", local);

	vector<SyncLogic::Action<Tag> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);
	if (actions.empty())
		return;

	TagProcessor processor(userModel);

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

		SyncState syncState;
		noteStore->GetSyncState(syncState);
		bool fullSync(userModel.GetLastSyncEnTime() < syncState.FullSyncBefore);

		syncLogger.BeginSyncStage(fullSync ? L"full" : L"incremental");

		EnInteropNoteList remoteNotes;
		NotebookList      remoteNotebooks;
		TagList           remoteTags;

		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);

		if (fullSync)
		{
			int globalUpdateCount(userModel.GetUpdateCount());
			if (globalUpdateCount < syncState.UpdateCount)
			{
				noteStore->ListEntries
					( remoteNotes
					, remoteNotebooks
					, remoteTags
					, notebook.guid
					);
			}
		}
		else
		{
			int notebookUpdateCount(userModel.GetNotebookUpdateCount(notebook.guid));
			if (notebookUpdateCount < syncState.UpdateCount)
			{
				int globalUpdateCount(userModel.GetUpdateCount());

				vector<Guid> expungedNotes;
				vector<Guid> expungedNotebooks;
				vector<Guid> expungedTags;
				vector<Guid> resources;

				noteStore->ListEntries
					( globalUpdateCount
					, notebookUpdateCount
					, remoteNotes
					, remoteNotebooks
					, remoteTags
					, expungedNotes
					, expungedNotebooks
					, expungedTags
					, resources
					, notebook.guid
					);

				syncLogger.ListGuids(L"Expunged notes",     expungedNotes);
				syncLogger.ListGuids(L"Expunged notebooks", expungedNotebooks);
				syncLogger.ListGuids(L"Expunged tags",      expungedTags);

				foreach (Guid & guid, expungedNotes)
				{
					try
					{
						userModel.DeleteNote(guid);
						syncLogger.Delete(guid);
					}
					catch (const std::exception &)
					{
						// eat deletion errors
					}
				}
				foreach (Guid & guid, expungedNotebooks)
				{
					userModel.DeleteNotebook(guid);
					syncLogger.Delete(guid);
				}
				foreach (Guid & guid, expungedTags)
				{
					userModel.DeleteTag(guid);
					syncLogger.Delete(guid);
				}
			}
		}

		syncLogger.ListNotes     (L"Remote notes",     remoteNotes);
		syncLogger.ListNotebooks (L"Remote notebooks", remoteNotebooks);
		syncLogger.ListTags      (L"Remote tags",      remoteTags);

		ProcessNotebooks (remoteNotebooks, *noteStore, fullSync);
		ProcessTags      (remoteTags,      *noteStore, fullSync);

		userModel.GetLastUsedNotebook(notebook);

		ProcessNotes(remoteNotes, *noteStore, notebook, fullSync);

		userModel.SetNotebookUpdateCount(notebook.guid, syncState.UpdateCount);
		userModel.SetUpdateCount(syncState.UpdateCount);
		userModel.SetLastSyncEnTime(syncState.CurrentEnTime);

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
