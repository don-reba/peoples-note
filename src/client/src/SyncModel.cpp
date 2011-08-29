#include "stdafx.h"
#include "SyncModel.h"

#include "DataStore.h"
#include "EnNoteTranslator.h"
#include "IEnService.h"
#include "IMessagePump.h"
#include "INoteStore.h"
#include "ILogger.h"
#include "IUserModel.h"
#include "IUserStore.h"
#include "NotebookProcessor.h"
#include "NoteProcessor.h"
#include "ScopedLock.h"
#include "SyncLogic.h"
#include "TagProcessor.h"
#include "Tools.h"
#include "UserUpdater.h"

#include <Evernote\EDAM\Error.h>

#include <algorithm>
#include <sstream>

using namespace std;
using namespace Tools;

//----------
// interface
//----------

SyncModel::SyncModel
	( EnNoteTranslator & enNoteTranslator
	, IEnService       & enService
	, IMessagePump     & messagePump
	, IUserModel       & userModel
	, ILogger          & logger
	)
	: enNoteTranslator (enNoteTranslator)
	, syncThread       (NULL)
	, enService        (enService)
	, messagePump      (messagePump)
	, userModel        (userModel)
	, stopRequested    (false)
	, logger           (logger)
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
		SyncMessageQueue::Message message(messages.Dequeue());
		switch (message.Type)
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
		case SyncMessageQueue::MessageSyncFailed:
			statusText = message.Text;
			SignalStatusUpdated();
			SignalSyncComplete();
			break;
		case SyncMessageQueue::MessageSyncProgress:
			syncProgress = message.Value;
			SignalStatusUpdated();
			break;
		case SyncMessageQueue::MessageText:
			statusText = message.Text;
			SignalStatusUpdated();
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

void SyncModel::BeginSync
	( const wstring & username
	, const wstring & password
	)
{
	CloseThread();
	this->username = username;
	this->password = password;
	syncThread = ::CreateThread
		( NULL             // lpsa
		, 0                // cbStack
		, &SyncModel::Sync // lpStartAddr
		, this             // lpvThreadParam
		, 0                // fdwCreate
		, NULL             // lpIDThread
		);
}

const wchar_t * SyncModel::GetStatusText()
{
	return statusText.c_str();
}

double SyncModel::GetSyncProgress()
{
	return syncProgress;
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
		notes.back().guid     = note.guid;
		notes.back().isDirty  = note.isDirty;
		notes.back().name     = note.name;
		notes.back().usn      = note.usn;

		userModel.GetNoteResources(note.guid, notes.back().resources);
	}
}

void SyncModel::FinishSync
	( const wchar_t * logMessage
	, const wchar_t * friendlyMessage
	)
{
	PostProgressMessage(0.0);
	if (*logMessage)
		logger.SyncError(logMessage);
	PostTextMessage(friendlyMessage);
	PostSyncCompleteMessage();
	userModel.Unload();
}

void SyncModel::PostProgressMessage(double progress)
{
	SyncMessageQueue::Message message
		( SyncMessageQueue::MessageSyncProgress
		, L""
		, progress
		);
	messages.Enqueue(message);
	messagePump.WakeUp();
}

void SyncModel::PostTextMessage(const wchar_t * text)
{
	SyncMessageQueue::Message message
		( SyncMessageQueue::MessageText
		, text
		, 0.0
		);
	messages.Enqueue(message);
	messagePump.WakeUp();
}

void SyncModel::PostSyncCompleteMessage()
{
	SyncMessageQueue::Message message
		( SyncMessageQueue::MessageSyncComplete
		, L""
		, 0.0
		);
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
	logger.ListNotes(L"Local notes", local);

	vector<SyncLogic::Action<EnInteropNote> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);

	NoteProcessor processor(enNoteTranslator, userModel, noteStore, notebook);

	logger.BeginSyncStage(L"notes");

	// get the notes to delete
	GuidList deletedNotes;
	userModel.GetDeletedNotes(deletedNotes);

	// count the number of valid actions
	double actionCount(deletedNotes.size());
	foreach (const SyncLogic::Action<EnInteropNote> action, actions)
	{
		// filter by notes from this notebook
		if (action.Local && action.Local->notebook != notebook.guid)
			continue;
		if (action.Remote && action.Remote->notebook != notebook.guid)
			continue;
		actionCount += 1.0;
	}
	if (actionCount == 0.0)
		return;
	double actionIndex(0.0);

	// sync deleted notes
	PostProgressMessage(0.0);
	PostTextMessage(L"Synchronizing deleted notes...");

	foreach (const Guid & note, deletedNotes)
	{
		if (!note.IsLocal())
			processor.DeleteRemote(note);

		PostProgressMessage(actionIndex / actionCount);
		actionIndex += 1.0;
	}

	// perform the actions
	PostProgressMessage(0.0);
	PostTextMessage(L"Synchronizing notes...");

	foreach (const SyncLogic::Action<EnInteropNote> action, actions)
	{
		// filter by notes from this notebook
		if (action.Local && action.Local->notebook != notebook.guid)
			continue;
		if (action.Remote && action.Remote->notebook != notebook.guid)
			continue;

		try
		{
			switch (action.Type)
			{
			case SyncLogic::ActionAdd:
				logger.PerformAction(L"Add", NULL, &action.Remote->guid);
				processor.AddLocal(*action.Remote);
				break;
			case SyncLogic::ActionDelete:
				logger.PerformAction(L"Delete", &action.Local->guid, NULL);
				processor.DeleteLocal(*action.Local);
				break;
			case SyncLogic::ActionMerge:
				logger.PerformAction(L"Merge", &action.Local->guid, &action.Remote->guid);
				processor.MergeLocal(*action.Local, *action.Remote);
				break;
			case SyncLogic::ActionUpload:
				if (action.Local->guid.IsLocal())
				{
					logger.PerformAction(L"Create", &action.Local->guid, NULL);
					processor.CreateRemote(*action.Local);
				}
				else
				{
					logger.PerformAction(L"Update", &action.Local->guid, NULL);
					processor.UpdateRemote(*action.Local);
				}
				break;
			}
		}
		catch (...)
		{
			ExceptionMessage message = logger.GetExceptionMessage();
			logger.SyncError(message.Message);
		}

		PostProgressMessage(actionIndex / actionCount);
		actionIndex += 1.0;
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
	logger.ListNotebooks(L"Local notebooks", local);

	vector<SyncLogic::Action<Notebook> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);
	if (actions.empty())
		return;

	PostProgressMessage(0.0);
	PostTextMessage(L"Synchronizing notebooks...");

	NotebookProcessor processor(userModel);

	logger.BeginSyncStage(L"notebooks");
	double actionIndex(0.0);
	foreach (const SyncLogic::Action<Notebook> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			logger.PerformAction(L"Add", NULL, &action.Remote->guid);
			processor.AddLocal(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			logger.PerformAction(L"Delete", &action.Local->guid, NULL);
			processor.DeleteLocal(*action.Local);
			break;
		case SyncLogic::ActionMerge:
			logger.PerformAction(L"Merge", &action.Local->guid, &action.Remote->guid);
			processor.MergeLocal(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			if (action.Local->guid.IsLocal())
			{
					logger.PerformAction(L"Create", &action.Local->guid, NULL);
					processor.CreateRemote(*action.Local, noteStore);
			}
			else
			{
					logger.PerformAction(L"Update", &action.Local->guid, NULL);
					processor.UpdateRemote(*action.Local, noteStore);
			}
			break;
		}

		PostProgressMessage(actionIndex / actions.size());
		actionIndex += 1.0;
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
	logger.ListTags(L"Local tags", local);

	vector<SyncLogic::Action<Tag> > actions;
	SyncLogic::Sync(fullSync, remote, local, actions);
	if (actions.empty())
		return;

	PostProgressMessage(0.0);
	PostTextMessage(L"Synchronizing tags...");

	TagProcessor processor(userModel);

	logger.BeginSyncStage(L"tags");
	double actionIndex(0.0);
	foreach (const SyncLogic::Action<Tag> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			logger.PerformAction(L"Add", NULL, &action.Remote->guid);
			processor.AddLocal(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			logger.PerformAction(L"Delete", &action.Local->guid, NULL);
			processor.DeleteLocal(*action.Local);
			break;
		case SyncLogic::ActionMerge:
			logger.PerformAction(L"Merge", &action.Local->guid, &action.Remote->guid);
			processor.MergeLocal(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			if (action.Local->guid.IsLocal())
			{
				logger.PerformAction(L"Create", &action.Local->guid, NULL);
				processor.CreateRemote(*action.Local, noteStore);
			}
			else
			{
				logger.PerformAction(L"Update", &action.Local->guid, NULL);
				processor.UpdateRemote(*action.Local, noteStore);
			}
			break;
		}

		PostProgressMessage(actionIndex / actions.size());
		actionIndex += 1.0;
	}
}

void SyncModel::Sync()
try
{
	LoggerRAII loggerRaii(logger);

	PostTextMessage(L"Connecting...");

	userModel.Load(username);

	IEnService::UserStorePtr userStore(enService.GetUserStore());
	IUserStore::AuthenticationResult authenticationResult
		(userStore->GetAuthenticationToken(username, password));
	if (!authenticationResult.IsGood)
	{
		FinishSync
			( authenticationResult.Message.c_str()
			, L"Tried to sync, but could not authenticate."
			);
		return;
	}

	IEnService::NoteStorePtr noteStore
		( enService.GetNoteStore
			( authenticationResult.Token
			, authenticationResult.ShardId
			)
		);

	if (userModel.GetSyncVersion() < 5)
	{
		UpdateModel(*noteStore);
		userModel.SetSyncVersion(5);
	}

	SyncState syncState;
	noteStore->GetSyncState(syncState);
	bool fullSync(userModel.GetLastSyncEnTime() < syncState.FullSyncBefore);

	logger.BeginSyncStage(fullSync ? L"full" : L"incremental");

	PostProgressMessage(0.0);
	PostTextMessage(fullSync ? L"Begin full sync..." : L"Begin incremental sync...");

	EnInteropNoteList remoteNotes;
	NotebookList      remoteNotebooks;
	TagList           remoteTags;

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	bool isLocalNotebook(notebook.guid.IsLocal());

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

			logger.ListGuids(L"Expunged notes",     expungedNotes);
			logger.ListGuids(L"Expunged notebooks", expungedNotebooks);
			logger.ListGuids(L"Expunged tags",      expungedTags);

			foreach (Guid & guid, expungedNotes)
			{
				try
				{
					logger.PerformAction(L"Delete", &guid, NULL);
					userModel.ExpungeNote(guid);
				}
				catch (const std::exception &)
				{
					// ignore deletion errors
				}
			}
			foreach (Guid & guid, expungedNotebooks)
			{
				logger.PerformAction(L"Delete", &guid, NULL);
				userModel.ExpungeNotebook(guid);
			}
			foreach (Guid & guid, expungedTags)
			{
				logger.PerformAction(L"Delete", &guid, NULL);
				userModel.ExpungeTag(guid);
			}
		}
	}

	logger.ListNotes     (L"Remote notes",     remoteNotes);
	logger.ListNotebooks (L"Remote notebooks", remoteNotebooks);
	logger.ListTags      (L"Remote tags",      remoteTags);

	ProcessNotebooks (remoteNotebooks, *noteStore, fullSync);
	ProcessTags      (remoteTags,      *noteStore, fullSync);

	UpdateDefaultNotebook(*noteStore);
	userModel.GetLastUsedNotebook(notebook);

	ProcessNotes(remoteNotes, *noteStore, notebook, fullSync);

	userModel.SetNotebookUpdateCount(notebook.guid, isLocalNotebook ? 0 : syncState.UpdateCount);
	userModel.SetUpdateCount(syncState.UpdateCount);
	userModel.SetLastSyncEnTime(syncState.CurrentEnTime);

	int dirtyNoteCount(userModel.GetDirtyNoteCount(notebook));

	FinishSync
		( L""
		, fullSync
		? L"Tip: choose a notebook and sync again to get the notes."
		: (dirtyNoteCount > 0)
			? L"Some notes could not be synchronized."
			: L""
		);
}
catch (...)
{
	ExceptionMessage message = logger.GetExceptionMessage();
	FinishSync(message.Message.c_str(), message.Title.c_str());
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	reinterpret_cast<SyncModel*>(param)->Sync();
	return 0;
}

void SyncModel::UpdateDefaultNotebook(INoteStore & noteStore)
{
	Guid defaultNotebook;
	noteStore.GetDefaultNotebook(defaultNotebook);
	userModel.MakeNotebookDefault(defaultNotebook);
}

void SyncModel::UpdateModel(INoteStore & noteStore)
{
	UserUpdater updater(noteStore, userModel);

	logger.BeginSyncStage(L"update");

	PostProgressMessage(0.0);
	PostTextMessage(L"Updating notes...");
	{
		NotebookList notebooks;
		userModel.GetNotebooks(notebooks);
		foreach (const Notebook & notebook, notebooks)
		{
			if (notebook.guid.IsLocal())
				continue;

			try { updater.UpdateNotebook(notebook.guid); }
			catch (const std::exception &) {}

			NoteList notes;
			userModel.GetNotesByNotebook(notebook, notes);
			double progress(0.0);
			foreach (const Note & note, notes)
			{
				if (note.guid.IsLocal())
					continue;

				try { updater.UpdateNote(note.guid); }
				catch (const std::exception &) {}

				progress += 1.0;
				PostProgressMessage(progress / notes.size());
			}
		}
	}

	PostProgressMessage(0.0);
	PostTextMessage(L"Updating tags...");
	{
		TagList tags;
		userModel.GetTags(tags);
		double progress(0.0);
		foreach (const Tag & tag, tags)
		{
			if (tag.guid.IsLocal())
				continue;

			try { updater.UpdateTag(tag.guid); }
			catch (const std::exception &) {}

			progress += 1.0;
			PostProgressMessage(progress / tags.size());
		}
	}
}

//--------------------------
// UserModel::LoggerRAII
//--------------------------

SyncModel::LoggerRAII::LoggerRAII(ILogger & logger)
	: logger (logger)
{
	logger.Clear();
}

SyncModel::LoggerRAII::~LoggerRAII()
{
	logger.Flush();
}
