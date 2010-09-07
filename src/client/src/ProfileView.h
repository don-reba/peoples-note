#pragma once

#include "HTMLayoutWindow.h"
#include "IProfileView.h"

class IHtmlDataLoader;

class ProfileView : public HTMLayoutWindow, public IProfileView
{
private:

	HINSTANCE instance;
	HWND      parent;

	signal SignalClose;

// interface

public:

	ProfileView(HINSTANCE instance);

	void Create(HWND parent);

// IProfileView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void Hide();

	virtual void Show();

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);
};
