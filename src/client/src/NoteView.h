#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HINSTANCE instance;
	HWND      parent;

// interface

public:

	NoteView(HINSTANCE instance);

	void Create(HWND parent);

// INoteView implementation

public:

	virtual void Hide();

	virtual void SetBody(std::wstring html);

	virtual void SetTitle(std::wstring text);

	virtual void Show();

// utility functions

private:

	void CopyParentSize();

	ATOM RegisterClass(std::wstring wndClass);

// window message handlers

private:

	void OnCommand(Msg<WM_COMMAND> & msg);

	virtual void ProcessMessage(WndMsg &msg);
};
