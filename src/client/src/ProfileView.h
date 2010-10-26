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
	signal SignalDbMove;

// interface

public:

	ProfileView(HINSTANCE instance, bool highRes);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// IProfileView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectDbMove(slot_type OnDbMove);

	virtual void Hide();

	virtual bool IsShown();

	virtual void SetDbPath(const std::wstring & path);

	virtual void SetDbSize(const std::wstring & size);

	virtual void SetMoveButtonText(const std::wstring & text);

	virtual void SetMoveErrorMessage(const std::wstring & message);

	virtual void SetUsername(const std::wstring & username);

	virtual void Show();

// window message handlers

private:

	void OnActivate(Msg<WM_ACTIVATE> & msg);
	void OnCommand (Msg<WM_COMMAND> & msg);
	void OnKeyUp   (Msg<WM_KEYUP>   & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnDbMove(BEHAVIOR_EVENT_PARAMS * params);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);
};
