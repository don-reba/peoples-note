#pragma once

#include "HTMLayoutWindow.h"
#include "IAboutView.h"

class IHtmlDataLoader;

class AboutView : public HTMLayoutWindow, public IAboutView
{
private:

	HINSTANCE instance;
	HWND      parent;

	signal SignalClose;

// interface

public:

	AboutView(HINSTANCE instance);

	void Create(HWND parent);

// IAboutView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void Hide();

	virtual void Show();

// window message handlers

private:

	void OnKeyUp(Msg<WM_KEYUP> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);
};
