#pragma once

#include "HTMLayoutWindow.h"
#include "ICredentialsView.h"

class CredentialsView : public HTMLayoutWindow, public ICredentialsView
{
// data

private:

	HWND           parent;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

	signal SignalCancel;
	signal SignalOk;

// interface

public:

	CredentialsView(HINSTANCE instance);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// ICredentialsView implementation

public:

	virtual void Close();

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectOk(slot_type OnOk);

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetUsername() const;

	virtual void Open();

	virtual void SetMessage(const std::wstring & message);

	virtual void SetPassword(const std::wstring & password);

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
	void OnSettingChange (Msg<WM_SETTINGCHANGE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnCancel (BEHAVIOR_EVENT_PARAMS * params);
	void OnOk     (BEHAVIOR_EVENT_PARAMS * params);
};
