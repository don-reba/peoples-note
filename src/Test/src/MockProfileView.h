#pragma once
#include "IProfileView.h"

class MockProfileView : public IProfileView
{
public:

	bool isShown;

	signal SignalClose;

public:

	MockProfileView();

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void Hide();

	virtual void Show();
};
