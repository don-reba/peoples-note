#pragma once

#include "ISignalProvider.h"

class IUserModel;

class ISyncModel : public ISignalProvider
{
public:

	virtual void BeginSync(IUserModel & userModel) = 0;

	virtual void ConnectSyncComplete(slot_type OnSyncComplete) = 0;
};
