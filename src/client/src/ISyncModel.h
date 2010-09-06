#pragma once

#include "ISignalProvider.h"

class IUserModel;

class ISyncModel : public ISignalProvider
{
public:

	virtual void BeginSync(const std::wstring & username) = 0;

	virtual void ConnectNotebooksChanged(slot_type OnNotebooksChanged) = 0;

	virtual void ConnectNotesChanged(slot_type OnNotesChanged) = 0;

	virtual void ConnectStatusUpdated(slot_type OnStatusUpdated) = 0;

	virtual void ConnectTagsChanged(slot_type OnTagsChanged) = 0;

	virtual void ConnectSyncComplete(slot_type OnSyncComplete) = 0;

	virtual const wchar_t * GetStatusText() = 0;

	virtual double GetSyncProgress() = 0;
};
