#include "stdafx.h"
#include "MockSyncModel.h"

#include "IUserModel.h"

using namespace std;

MockSyncModel::MockSyncModel()
	: syncBegan (false)
{
}

void MockSyncModel::BeginSync(const wstring & username)
{
	this->syncBegan = true;
	this->username  = username;
}

void MockSyncModel::ConnectNotebooksChanged(slot_type OnNotebooksChanged)
{
	SignalNotebooksChanged.connect(OnNotebooksChanged);
}

void MockSyncModel::ConnectNotesChanged(slot_type OnNotesChanged)
{
	SignalNotesChanged.connect(OnNotesChanged);
}

void MockSyncModel::ConnectStatusUpdated(slot_type OnStatusUpdated)
{
	SignalStatusUpdated.connect(OnStatusUpdated);
}

void MockSyncModel::ConnectSyncComplete(slot_type OnSyncComplete)
{
	SignalSyncComplete.connect(OnSyncComplete);
}

void MockSyncModel::ConnectTagsChanged(slot_type OnTagsChanged)
{
	SignalTagsChanged.connect(OnTagsChanged);
}

const wchar_t * MockSyncModel::GetStatusText()
{
	return statusText.c_str();
}
