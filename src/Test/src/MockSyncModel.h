#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
public:

	signal SignalSyncComplete;

	bool syncBegan;

public:

	MockSyncModel();

	virtual void BeginSync();

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);
};
