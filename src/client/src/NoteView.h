#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"
#include "Note.h"
#include "NoteViewGestureProcessor.h"
#include "WindowRenderer.h"

class IAnimator;

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HWND      parent;
	HINSTANCE instance;

	NoteViewGestureProcessor gestureProcessor;

	Note note;

	POINT startScrollPos;

	bool isDirty;
	bool isMaximized;

	signal SignalClose;
	signal SignalEdit;
	signal SignalToggleMaximize;

// interface

public:

	NoteView
		( HINSTANCE   instance
		, bool        highRes
		, IAnimator & animator);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// INoteView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectEdit(slot_type OnEdit);

	virtual void ConnectToggleMaximize(slot_type OnToggleMaximize);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual bool IsDirty();

	virtual bool IsMaximized();

	virtual void MaximizeWindow();

	virtual void Render(Thumbnail & thumbnail);

	virtual void RestoreWindow();

	virtual void SetNote
		( const Note         & note
		, const std::wstring & titleText
		, const std::wstring & subtitleText
		, const std::wstring & bodyHtml
		);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();

// utility functions

private:

	POINT GetScrollPos();

	ATOM RegisterClass(const std::wstring & wndClass);

	void SetScrollPos(POINT pos);

	void UpdateScrollbar();

	void UpdateWindowState();

// gesture message handlers

private:

	void OnDelayedMouseDown();
	void OnGestureStart();
	void OnGestureStep();

// window message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnClose    (Msg<WM_CLOSE>    & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);
	void OnMouseDown      (Msg<WM_LBUTTONDOWN>    & msg);
	void OnMouseUp        (Msg<WM_LBUTTONUP>      & msg);
	void OnMouseMove      (Msg<WM_MOUSEMOVE>      & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnEdit   (BEHAVIOR_EVENT_PARAMS * params);
	void OnToggle (BEHAVIOR_EVENT_PARAMS * params);
	void OnHome   (BEHAVIOR_EVENT_PARAMS * params);
	void OnInput  (BEHAVIOR_EVENT_PARAMS * params);
};
