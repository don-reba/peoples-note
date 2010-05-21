#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"
#include "WindowRenderer.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HINSTANCE instance;
	HWND      parent;

	bool isFullScreen;

	WindowRenderer windowRenderer;

	Blob blob;

	DataSignal SignalLoadingData;

// interface

public:

	NoteView(HINSTANCE instance);

	void Create(HWND parent);

	void Destroy();

	virtual void RegisterEventHandlers();

// INoteView implementation

public:

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void Hide();

	virtual void Render(Thumbnail & thumbnail);

	virtual void SetBody(const std::wstring & html);

	virtual void SetSubtitle(const std::wstring & text);

	virtual void SetTitle(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();

// utility functions

private:

	void CopyParentSize();

	ATOM RegisterClass(const std::wstring & wndClass);

	void ToggleFullScreen();

// window message handlers

private:

	void OnClose(Msg<WM_CLOSE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnFullScreen (BEHAVIOR_EVENT_PARAMS * params);
	void OnHome       (BEHAVIOR_EVENT_PARAMS * params);

	virtual BOOL OnLoadData(NMHL_LOAD_DATA * params);
};
