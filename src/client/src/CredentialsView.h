#pragma once

#include "HTMLayoutWindow.h"
#include "ICredentialsView.h"

class IHtmlDataLoader;

class CredentialsView : public HTMLayoutWindow, public ICredentialsView
{
// events

	MacroEvent(Cancel)
	MacroEvent(Created)
	MacroEvent(Ok)

// data

private:

	HWND           parent;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

// interface

public:

	CredentialsView
		( HINSTANCE         instance
		, bool              highRes
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create(HWND parent);

// ICredentialsView implementation

public:

	virtual void Close();

	virtual std::wstring GetShard() const;

	virtual std::wstring GetToken() const;

	virtual std::wstring GetUsername() const;

	virtual void Open();

	virtual void SetFocusToUsername();

	virtual void SetMessage(const std::wstring & message);

	virtual void SetShard(const std::wstring & shard);

	virtual void SetToken(const std::wstring & token);

	virtual void SetUsername(const std::wstring & username);

// utility functions

private:

	void CopyParentSize();

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();

// window message handlers

private:

	void OnActivate      (Msg<WM_ACTIVATE>      & msg);
	void OnCommand       (Msg<WM_COMMAND>       & msg);
	void OnKeyUp         (Msg<WM_KEYUP>         & msg);
	void OnSettingChange (Msg<WM_SETTINGCHANGE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	virtual BOOL OnKey (KEY_PARAMS * params);
};
