#pragma once

#include "ISignalProvider.h"

class IAppModel : public ISignalProvider
{
public:

	virtual void ConnectStart(slot_type OnStart) = 0;
};
