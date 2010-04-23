#pragma once

#include "IMessagePump.h"

class MessagePump : public IMessagePump
{
private:

	DWORD thread;

public:

	MessagePump();

	virtual void WakeUp();
};
