#pragma once

#include "ISyncModel.h"

#include "EnInteropNote.h"
#include "Note.h"
#include "Notebook.h"
#include "SyncMessageQueue.h"
#include "Tag.h"

class IEnService;
class IMessagePump;
class INoteStore;
class ISyncLogger;
class IUserModel;

class SyncModel : public ISyncModel
{
// data

private:

	IEnService   & enService;
	IMessagePump & messagePump;
	ISyncLogger  & syncLogger;
	IUserModel   & userModel;

	HANDLE syncThread;

	CRITICAL_SECTION lock;

	SyncMessageQueue messages;

	bool stopRequested;

	signal SignalNotebooksChanged;
	signal SignalNotesChanged;
	signal SignalTagsChanged;
	signal SignalSyncComplete;

// interface

public:

	SyncModel
		( IEnService   & enService
		, IMessagePump & messagePump
		, IUserModel   & userModel
		, ISyncLogger  & logger
		);

	~SyncModel();

	void ProcessMessages();

	void StopSync();

// ISyncModel implementation

public:

	virtual void BeginSync(const std::wstring & username);

	virtual void ConnectNotebooksChanged(slot_type OnNotebooksChanged);

	virtual void ConnectNotesChanged(slot_type OnNotesChanged);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);

	virtual void ConnectTagsChanged(slot_type OnTagsChanged);

// utility functions

private:

	void CloseThread();

	void PostMessage(SyncMessageQueue::Message message);

	void Sync();

	static DWORD WINAPI Sync(LPVOID param);

	void ProcessNotes
		( const EnInteropNoteList & remoteNotes
		, INoteStore              & noteStore
		, Notebook                & notebook
		);

	void ProcessNotebooks
		( const NotebookList & remoteNotebooks
		, INoteStore         & noteStore
		);

	void ProcessTags
		( const TagList & remoteTags
		, INoteStore    & noteStore
		);
};
