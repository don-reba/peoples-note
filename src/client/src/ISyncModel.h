#pragma once

#include "ISignalProvider.h"

class ISyncModel : public ISignalProvider
{
public:

	virtual void BeginSync() = 0;

	virtual void ConnectSyncComplete(slot_type OnSyncComplete) = 0;
};
