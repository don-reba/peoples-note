#include "stdafx.h"
#include "MockAppModel.h"

void MockAppModel::ConnectStart(slot_type OnStart)
{
	SignalStart.connect(OnStart);
}

void MockAppModel::Start()
{
	SignalStart();
}
