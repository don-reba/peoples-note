#pragma once

#include "HTMLayoutWindow.h"
#include "IAboutView.h"

class IHtmlDataLoader;

class AboutView : public HTMLayoutWindow, public IAboutView
{
// events

	MacroEvent(Close)

// data

private:

	HWND      parent;
	HWND      menuBar;
	HINSTANCE instance;

// interface

public:

	AboutView
		( HINSTANCE         instance
		, bool              highRes
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create(HWND parent);

// IAboutView implementation

public:

	virtual void Hide();

	virtual void Show();

// window message handlers

private:

	void OnCommand (Msg<WM_COMMAND> & msg);
	void OnKeyUp   (Msg<WM_KEYUP>   & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
