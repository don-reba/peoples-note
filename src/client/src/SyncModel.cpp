#include "stdafx.h"
#include "SyncModel.h"

#include "ScopedLock.h"

//----------
// interface
//----------

SyncModel::SyncModel()
	: stopRequested (false)
	, syncThread    (NULL)
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
	while (!messages.empty())
	{
		switch (messages.front())
		{
		case MessageSyncComplete:
			SignalSyncComplete();
			break;
		}
		messages.pop();
	}
}

void SyncModel::StopSync()
{
	{
		ScopedLock lock(lock);
		stopRequested = true;
	}
	::WaitForSingleObject(syncThread, INFINITE);
}

//--------------------------
// ISyncModel implementation
//--------------------------

void SyncModel::BeginSync()
{
	CloseThread();
	stopRequested = false;
	syncThread = ::CreateThread
		( NULL             // lpsa
		, 0                // cbStack
		, &SyncModel::Sync // lpStartAddr
		, this             // lpvThreadParam
		, 0                // fdwCreate
		, NULL             // lpIDThread
		);
}

void SyncModel::ConnectSyncComplete(slot_type OnSyncComplete)
{
	SignalSyncComplete.connect(OnSyncComplete);
}

//------------------
// utility functions
//------------------

void SyncModel::CloseThread()
{
	if (!syncThread)
		return;
	StopSync();
	::CloseHandle(syncThread);
	syncThread = NULL;
}

DWORD SyncModel::Sync()
{
	{
		ScopedLock lock(lock);
		messages.push(MessageSyncComplete);
	}
	return 0;
}

DWORD WINAPI SyncModel::Sync(LPVOID param)
{
	return reinterpret_cast<SyncModel*>(param)->Sync();
}
