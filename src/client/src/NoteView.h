#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HINSTANCE instance;
	HWND      parent;

	bool isFullScreen;

	Blob blob;

	DataSignal SignalLoadingData;

// interface

public:

	NoteView(HINSTANCE instance);

	void Create(HWND parent);

// INoteView implementation

public:

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void Hide();

	virtual void SetBody(std::wstring html);

	virtual void SetSubtitle(std::wstring text);

	virtual void SetTitle(std::wstring text);

	virtual void Show();

// utility functions

private:

	void CopyParentSize();

	ATOM RegisterClass(std::wstring wndClass);

	void ToggleFullScreen();

// window message handlers

private:

	void OnClose(Msg<WM_CLOSE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

protected:

	void OnFullScreen();

	void OnHome();

	BOOL OnLoadData(NMHL_LOAD_DATA * params);
};
