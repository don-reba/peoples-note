#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
public:

	signal SignalSyncComplete;
	signal SignalNotebooksChanged;
	signal SignalNotesChanged;
	signal SignalStatusUpdated;
	signal SignalTagsChanged;

	bool syncBegan;

	std::wstring documentPath;
	std::wstring username;
	std::wstring password;
	std::wstring statusText;
	double       syncProgress;

public:

	MockSyncModel();

	virtual void BeginSync
		( const std::wstring & username
		, const std::wstring & pasword
		);

	virtual void ConnectNotebooksChanged(slot_type OnNotebooksChanged);

	virtual void ConnectNotesChanged(slot_type OnNotesChanged);

	virtual void ConnectStatusUpdated(slot_type OnStatusUpdated);

	virtual void ConnectTagsChanged(slot_type OnTagsChanged);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);

	virtual const wchar_t * GetStatusText();

	virtual double GetSyncProgress();
};
