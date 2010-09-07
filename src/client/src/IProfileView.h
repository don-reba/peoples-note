#pragma once

#include "ISignalProvider.h"

class IProfileView : public ISignalProvider
{
public:

	virtual void ConnectClose(slot_type OnClose) = 0;

	virtual void Hide() = 0;

	virtual void Show() = 0;
};
