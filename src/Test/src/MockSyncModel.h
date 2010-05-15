#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
public:

	signal SignalSyncComplete;
	signal SignalNotebooksChanged;
	signal SignalNotesChanged;
	signal SignalTagsChanged;

	bool syncBegan;

	std::wstring documentPath;
	std::wstring username;

public:

	MockSyncModel();

	virtual void BeginSync(const std::wstring & username);

	virtual void ConnectNotebooksChanged(slot_type OnNotebooksChanged);

	virtual void ConnectNotesChanged(slot_type OnNotesChanged);

	virtual void ConnectTagsChanged(slot_type OnTagsChanged);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);
};
