#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
public:

	signal SignalSyncComplete;

	bool syncBegan;

	std::wstring documentPath;
	std::wstring username;

public:

	MockSyncModel();

	virtual void BeginSync(IUserModel & userModel);

	virtual void ConnectSyncComplete(slot_type OnSyncComplete);
};
