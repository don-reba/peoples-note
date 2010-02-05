#pragma once

#include "IAppModel.h"

class MockAppModel : public IAppModel
{
private:
	signal SignalStart;

public:

	virtual void ConnectStart(slot_type OnStart);

	void Start();
};
