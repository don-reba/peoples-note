#pragma once

#include "IMessagePump.h"

class MockMessagePump : public IMessagePump
{
public:

	bool wokeUp;

public:

	MockMessagePump();

	virtual void WakeUp();
};
