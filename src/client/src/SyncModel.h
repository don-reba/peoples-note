#pragma once

#include "ISyncModel.h"

#include <boost/scoped_ptr.hpp>
#include <queue>

class IEnService;
class IMessagePump;
class ISyncLogger;

class SyncModel : public ISyncModel
{
// nested types

private:

	enum Message
	{
		MessageSyncFailed,
		MessageSyncComplete,
	};

	class SyncContext
	{
	private:

		SyncModel   & syncModel;
		IUserModel  & userModel;
		ISyncLogger & syncLogger;
		bool stopRequested;

	public:

		SyncContext
			( SyncModel   & syncModel
			, IUserModel  & userModel
			, ISyncLogger & syncLogger
			);

		void EnqueueMessage(Message message);
		IEnService  & GetEnService();
		IUserModel  & GetUserModel();
		ISyncLogger & GetSyncLogger();
		bool GetStopRequested() const;
		void SetStopRequested(bool vaue);
	};

// data

private:

	IEnService   & enService;
	IMessagePump & messagePump;
	ISyncLogger  & syncLogger;

	HANDLE syncThread;

	CRITICAL_SECTION lock;

	boost::scoped_ptr<SyncContext> syncContext;

	std::queue<Message> messages;

	signal SignalSyncComplete;

// interface

public:

	SyncModel
		( IEnService   & enService
		, IMessagePump & messagePump
		, ISyncLogger  & logger
		);

	~SyncModel();

	void ProcessMessages();

	void StopSync();

// ISyncModel implementation

public:

	virtual void BeginSync(IUserModel & userModel);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);

// utility functions

private:

	void CloseThread();

	static DWORD WINAPI Sync(LPVOID param);
};
