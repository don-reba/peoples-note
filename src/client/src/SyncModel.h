#pragma once

#include "ISyncModel.h"

#include <queue>

class SyncModel : public ISyncModel
{
// nested types

private:

	enum Message
	{
		MessageSyncComplete
	};

// data

private:

	HANDLE syncThread;

	CRITICAL_SECTION lock;

	bool stopRequested;

	std::queue<Message> messages;

	signal SignalSyncComplete;

// interface

public:

	SyncModel();

	~SyncModel();

	void ProcessMessages();

	void StopSync();

// ISyncModel implementation

public:

	virtual void BeginSync();

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);

// utility functions

private:

	void CloseThread();

	DWORD Sync();

	static DWORD WINAPI Sync(LPVOID param);
};
