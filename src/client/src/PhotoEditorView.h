#pragma once

#include "HTMLayoutWindow.h"
#include "IPhotoEditorView.h"

class PhotoEditorView : public HTMLayoutWindow, public IPhotoEditorView
{
// data

private:

	HWND           parent;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

	std::wstring photoPath;

	signal SignalCancel;
	signal SignalOk;
	signal SignalCapture;

// interface

public:

	PhotoEditorView(HINSTANCE instance, bool highRes);

	void Create(HWND parent);

// IPhotoEditorView implementation

public:

	virtual void ConnectCapture(slot_type OnCapture);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectOk(slot_type OnOk);

	virtual void Hide();

	virtual void InitiateCapture();

	virtual std::wstring GetImagePath();

	virtual std::wstring GetTitle();

	virtual void Show();

// windows message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
