#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"
#include "WindowRenderer.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HINSTANCE instance;
	HWND      parent;

	bool isDirty;
	bool isFullScreen;

	WindowRenderer windowRenderer;

	Blob blob;

	signal     SignalClose;
	DataSignal SignalLoadingData;

// interface

public:

	NoteView(HINSTANCE instance);

	void Create(HWND parent);

	void Destroy();

	virtual void RegisterEventHandlers();

// INoteView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void GetBody(std::wstring & html);

	virtual void Hide();

	virtual bool IsDirty();

	virtual void Render(Thumbnail & thumbnail);

	virtual void SetBody(const std::wstring & html);

	virtual void SetSubtitle(const std::wstring & text);

	virtual void SetTitle(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();

// utility functions

private:

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
	void OnInput      (BEHAVIOR_EVENT_PARAMS * params);

	virtual BOOL OnLoadData(NMHL_LOAD_DATA * params);
};
