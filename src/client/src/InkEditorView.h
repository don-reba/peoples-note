#pragma once
#include "IInkEditorView.h"

#include "window.h"

class InkEditorView : public Window, public IInkEditorView
{
// data

private:

	SHACTIVATEINFO activateInfo;
	HINSTANCE      instance;
	HBITMAP        bmp;
	HDC            bmpDc;
	HWND           menuBar;
	HWND           parent;

	POINT lineStart;

	signal SignalAccept;
	signal SignalCancel;

// interface

public:

	InkEditorView(HINSTANCE instance);

	void Create(HWND parent);

// IInkEditorView implementation

public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void Hide();

	virtual void Show();

// window message handlers

private:

	void OnActivate  (Msg<WM_ACTIVATE>      & msg);
	void OnCommand   (Msg<WM_COMMAND>       & msg);
	void OnMouseDown (Msg<WM_LBUTTONDOWN>   & msg);
	void OnMouseMove (Msg<WM_MOUSEMOVE>     & msg);
	void OnMouseUp   (Msg<WM_LBUTTONUP>     & msg);
	void OnPaint     (Msg<WM_PAINT>         & msg);
	void OnSize      (Msg<WM_SIZE>          & msg);

	virtual void ProcessMessage(WndMsg &msg);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);
};
