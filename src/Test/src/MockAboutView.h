#pragma once

#include "IAboutView.h"

class MockAboutView : public IAboutView
{
public:

	bool isShown;

	signal SignalClose;

public:

	MockAboutView();

	virtual void ConnectClose(slot_type OnClose);

	virtual void Hide();

	virtual void Show();
};
