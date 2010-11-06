#pragma once

#include "ISyncModel.h"

#include "EnInteropNote.h"
#include "Note.h"
#include "Notebook.h"
#include "SyncMessageQueue.h"
#include "Tag.h"

class EnNoteTranslator;
class IEnService;
class IMessagePump;
class INoteStore;
class ISyncLogger;
class IUserModel;


class SyncModel : public ISyncModel
{
// data

private:

	EnNoteTranslator & enNoteTranslator;
	IEnService       & enService;
	IMessagePump     & messagePump;
	ISyncLogger      & syncLogger;
	IUserModel       & userModel;

	HANDLE syncThread;

	CRITICAL_SECTION lock;

	SyncMessageQueue messages;

	bool stopRequested;

	std::wstring statusText;
	double       syncProgress;

	signal SignalNotebooksChanged;
	signal SignalNotesChanged;
	signal SignalTagsChanged;
	signal SignalStatusUpdated;
	signal SignalSyncComplete;

// interface

public:

	SyncModel
		( EnNoteTranslator & enNoteTranslator
		, IEnService       & enService
		, IMessagePump     & messagePump
		, IUserModel       & userModel
		, ISyncLogger      & logger
		);

	~SyncModel();

	void ProcessMessages();

	void StopSync();

// ISyncModel implementation

public:

	virtual void BeginSync(const std::wstring & username);

	virtual void ConnectNotebooksChanged(slot_type OnNotebooksChanged);

	virtual void ConnectNotesChanged(slot_type OnNotesChanged);

	virtual void ConnectStatusUpdated(slot_type OnStatusUpdated);

	virtual void ConnectTagsChanged(slot_type OnTagsChanged);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);

	virtual const wchar_t * GetStatusText();

	virtual double GetSyncProgress();

// utility functions

private:

	void CloseThread();

	void GetNotes
		( const Notebook    & notebook
		, EnInteropNoteList & notes
		);

	void FinishSync
		( const wchar_t * logMessage
		, const wchar_t * friendlyMessage
		);

	void PostProgressMessage     (double          progress);
	void PostTextMessage         (const wchar_t * text);
	void PostSyncCompleteMessage ();

	void ProcessNotes
		( const EnInteropNoteList & remoteNotes
		, INoteStore              & noteStore
		, Notebook                & notebook
		, bool                      fullSync
		);

	void ProcessNotebooks
		( const NotebookList & remoteNotebooks
		, INoteStore         & noteStore
		, bool                 fullSync
		);

	void ProcessTags
		( const TagList & remoteTags
		, INoteStore    & noteStore
		, bool            fullSync
		);

	void Sync();

	static DWORD WINAPI Sync(LPVOID param);

	void UpdateDefaultNotebook(INoteStore & noteStore);
};
