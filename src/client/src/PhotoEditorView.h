#pragma once
#include "IPhotoEditorView.h"

#include "HTMLayoutWindow.h"

class PhotoEditorView : public HTMLayoutWindow, public IPhotoEditorView
{
	MacroEvent(Cancel)
	MacroEvent(Ok)

// data

private:

	HWND           parent;
	HWND           menuBar;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

// interface

public:

	PhotoEditorView(HINSTANCE instance, bool highRes);

	void Create(HWND parent);

// IPhotoEditorView implementation

public:

	virtual int CapturePhoto(int quality, int width, int height, std::wstring & path);

	virtual PhotoQuality GetQuality();

	virtual PhotoResolution GetResolution();

	virtual std::wstring GetTitle();

	virtual void Hide();

	virtual void SetQuality(PhotoQuality quality);

	virtual void SetMessage(const std::wstring & message);

	virtual void SetResolution(PhotoResolution resolution);

	virtual void Show();

// windows message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);
	void OnKeyUp    (Msg<WM_KEYUP>    & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
