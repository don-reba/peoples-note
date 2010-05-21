#include "stdafx.h"
#include "SyncModel.h"

#include "DataStore.h"
#include "IEnService.h"
#include "IMessagePump.h"
#include "INoteStore.h"
#include "ISyncLogger.h"
#include "IUserModel.h"
#include "IUserStore.h"
#include "ScopedLock.h"
#include "SyncLogic.h"
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
		PostMessage(SyncMessageQueue::MessageSyncFailed);
		userModel.Unload();
	}
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	reinterpret_cast<SyncModel*>(param)->Sync();
	return 0;
}

//----------------
// note processing
//----------------

void SyncModel::ProcessNotes
	( const EnInteropNoteList & remote
	, INoteStore              & noteStore
	, Notebook                & notebook
	)
{
	const EnInteropNoteList local; // TODO: initialize
	syncLogger.ListLocalNotes(local);

	vector<SyncLogic::Action<EnInteropNote> > actions;
	SyncLogic::FullSync(remote, local, actions);

	syncLogger.BeginSyncStage(L"notes");
	foreach (const SyncLogic::Action<EnInteropNote> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			AddNote(*action.Remote, noteStore, notebook);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			DeleteNote(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			RenameAddNotes(*action.Local, *action.Remote, noteStore, notebook);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			UploadNote(*action.Local, noteStore, notebook);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			MergeNotes(*action.Local, *action.Remote);
			break;
		}
	}
}

void SyncModel::AddNote
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

void SyncModel::DeleteNote(const EnInteropNote & local)
{
	userModel.DeleteNote(local.note);
}

void SyncModel::RenameAddNotes
	( const EnInteropNote & local
	, const EnInteropNote & remote
	, INoteStore          & noteStore
	, Notebook            & notebook
	)
{
	// note names need not be unique
	AddNote(remote, noteStore, notebook);
}

void SyncModel::UploadNote
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

void SyncModel::MergeNotes
	( const EnInteropNote & local
	, const EnInteropNote & remote
	)
{
	// keep local
}

//--------------------
// notebook processing
//--------------------

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

	syncLogger.BeginSyncStage(L"notebooks");
	foreach (const SyncLogic::Action<Notebook> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			AddNotebook(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			DeleteNotebook(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			RenameAddNotebooks(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			UploadNotebook(*action.Local, noteStore);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			MergeNotebooks(*action.Local, *action.Remote);
			break;
		}
	}
}

void SyncModel::AddNotebook(const Notebook & remote)
{
	userModel.AddNotebook(remote);
}

void SyncModel::DeleteNotebook(const Notebook & local)
{
	userModel.DeleteNotebook(local);
}

void SyncModel::RenameAddNotebooks
	( const Notebook & local
	, const Notebook & remote
	)
{
	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);

	vector<wstring> names;
	names.reserve(notebooks.size());
	foreach (const Notebook & notebook, notebooks)
		names.push_back(notebook.name);
	sort(names.begin(), names.end());

	int n(2);
	wstringstream name;
	do
	{
		name.str(wstring());
		name << local.name << L'(' << n << L')';
		++n;
	}
	while (binary_search(names.begin(), names.end(), name.str()));

	Notebook notebook = local;
	notebook.name = name.str();
	userModel.AddNotebook(notebook);
	userModel.AddNotebook(remote);
}

void SyncModel::UploadNotebook
	( const Notebook & local
	, INoteStore     & noteStore
	)
{
	Transaction transaction(userModel);

	Notebook replacement;
	noteStore.CreateNotebook(local, replacement);
	userModel.DeleteNotebook(local);
	userModel.AddNotebook(replacement);
}

void SyncModel::MergeNotebooks
	( const Notebook & local
	, const Notebook & remote
	)
{
	userModel.UpdateNotebook(local, remote);
}

//---------------
// tag processing
//---------------

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

	foreach (const SyncLogic::Action<Tag> action, actions)
	{
		switch (action.Type)
		{
		case SyncLogic::ActionAdd:
			syncLogger.Add(action.Remote->guid);
			AddTag(*action.Remote);
			break;
		case SyncLogic::ActionDelete:
			syncLogger.Delete(action.Local->guid);
			DeleteTag(*action.Local);
			break;
		case SyncLogic::ActionRenameAdd:
			syncLogger.Rename(action.Local->guid);
			syncLogger.Add(action.Remote->guid);
			RenameAddTags(*action.Local, *action.Remote);
			break;
		case SyncLogic::ActionUpload:
			syncLogger.Upload(action.Local->guid);
			UploadTag(*action.Local, noteStore);
			break;
		case SyncLogic::ActionMerge:
			syncLogger.Merge(action.Local->guid, action.Remote->guid);
			MergeTags(*action.Local, *action.Remote);
			break;
		}
	}
}

void SyncModel::AddTag(const Tag & remote)
{
	userModel.AddTag(remote);
}

void SyncModel::DeleteTag(const Tag & local)
{
	userModel.DeleteTag(local);
}

void SyncModel::RenameAddTags
		( const Tag & local
		, const Tag & remote
		)
{
	TagList tags;
	userModel.GetTags(tags);

	vector<wstring> names;
	names.reserve(tags.size());
	foreach (const Tag & tag, tags)
		names.push_back(tag.name);
	sort(names.begin(), names.end());

	int n(2);
	wstringstream name;
	do
	{
		name.str(wstring());
		name << local.name << L'(' << n << L')';
		++n;
	}
	while (binary_search(names.begin(), names.end(), name.str()));

	Tag tag;
	tag.name = name.str();
	userModel.AddTag(tag);
	userModel.AddTag(remote);
}

void SyncModel::UploadTag
		( const Tag & local
		, INoteStore          & noteStore
		)
{
	Transaction transaction(userModel);

	Tag replacement;
	noteStore.CreateTag(local, replacement);
	userModel.DeleteTag(local);
	userModel.AddTag(replacement);
}

void SyncModel::MergeTags
		( const Tag & local
		, const Tag & remote
		)
{
	userModel.UpdateTag(local, remote);
}
