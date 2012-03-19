#pragma once
#include "IProfileView.h"

#include "HTMLayoutWindow.h"

class IHtmlDataLoader;

class ProfileView : public HTMLayoutWindow, public IProfileView
{
	MacroEvent(Close)
	MacroEvent(Compact)
	MacroEvent(DbMove)

private:

	HWND      parent;
	HWND      menuBar;
	HINSTANCE instance;

// interface

public:

	ProfileView
		( HINSTANCE         instance
		, bool              highRes
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// IProfileView implementation

public:

	virtual void DisableMoveButton();

	virtual void EnableMoveButton();

	virtual void Hide();

	virtual bool IsShown();

	virtual void SetDbPath(const std::wstring & path);

	virtual void SetDbSize(const std::wstring & size);

	virtual void SetMessage(const std::wstring & message);

	virtual void SetMoveButtonText(const std::wstring & text);

	virtual void SetUsername(const std::wstring & username);

	virtual void Show();

// window message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);
	void OnKeyUp    (Msg<WM_KEYUP>    & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnCompact (BEHAVIOR_EVENT_PARAMS * params);
	void OnDbMove  (BEHAVIOR_EVENT_PARAMS * params);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
