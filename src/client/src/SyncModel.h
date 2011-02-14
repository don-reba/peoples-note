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
class ILogger;
class IUserModel;


class SyncModel : public ISyncModel
{
// events

	MacroEvent(NotebooksChanged)
	MacroEvent(NotesChanged)
	MacroEvent(StatusUpdated)
	MacroEvent(TagsChanged)
	MacroEvent(SyncComplete)

// types

private:

	struct ExceptionMessage
	{
		std::wstring Title;
		std::wstring Message;

		ExceptionMessage
			( const std::wstring & title
			, const std::wstring & message
			)
			: Title   (title)
			, Message (message)
		{
		}
	};

	class SyncLoggerRAII
	{
	private:

		ILogger & logger;

	public:

		SyncLoggerRAII(ILogger & logger);

		~SyncLoggerRAII();
	};

// data

private:

	EnNoteTranslator & enNoteTranslator;
	IEnService       & enService;
	IMessagePump     & messagePump;
	ILogger      & logger;
	IUserModel       & userModel;

	HANDLE syncThread;

	CRITICAL_SECTION lock;

	SyncMessageQueue messages;

	bool stopRequested;

	std::wstring statusText;
	double       syncProgress;

// interface

public:

	SyncModel
		( EnNoteTranslator & enNoteTranslator
		, IEnService       & enService
		, IMessagePump     & messagePump
		, IUserModel       & userModel
		, ILogger      & logger
		);

	~SyncModel();

	void ProcessMessages();

	void StopSync();

// ISyncModel implementation

public:

	virtual void BeginSync(const std::wstring & username);

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

	ExceptionMessage GetExceptionMessage();

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
