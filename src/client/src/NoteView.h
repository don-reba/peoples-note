#pragma once

#include "HTMLayoutWindow.h"
#include "INoteView.h"
#include "Note.h"
#include "WindowRenderer.h"

class NoteView : public HTMLayoutWindow, public INoteView
{
private:

	HWND      parent;
	HINSTANCE instance;

	Note note;

	bool isDirty;
	bool isFullScreen;

	signal SignalClose;
	signal SignalEdit;

// interface

public:

	NoteView(HINSTANCE instance, bool highRes);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// INoteView implementation

public:

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectEdit(slot_type OnEdit);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual bool IsDirty();

	virtual void Render(Thumbnail & thumbnail);

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

	ATOM RegisterClass(const std::wstring & wndClass);

	void UpdateFullScreen();

// window message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnClose    (Msg<WM_CLOSE>    & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnEdit       (BEHAVIOR_EVENT_PARAMS * params);
	void OnFullScreen (BEHAVIOR_EVENT_PARAMS * params);
	void OnHome       (BEHAVIOR_EVENT_PARAMS * params);
	void OnInput      (BEHAVIOR_EVENT_PARAMS * params);
};
