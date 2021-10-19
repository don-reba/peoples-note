#pragma once
#include "IVoiceEditorView.h"

#include "HTMLayoutWindow.h"

class VoiceEditorView : public HTMLayoutWindow, public IVoiceEditorView
{
	MacroEvent(Cancel)
	MacroEvent(Hide)
	MacroEvent(Play)
	MacroEvent(Show)
	MacroEvent(Stop)
	MacroEvent(Record)

// data

private:

	HWND           parent;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

// interface

public:

	VoiceEditorView
		( HINSTANCE         instance
		, bool              highRes
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

	virtual void SetButtons(int buttons);

// IVoiceEditorView implementation

public:

	virtual void Hide();

	virtual void Show();

// windows message handlers

private:

	void OnActivate (Msg<WM_ACTIVATE> & msg);
	void OnClose    (Msg<WM_CLOSE>    & msg);
	void OnCommand  (Msg<WM_COMMAND>  & msg);
	void OnKeyUp    (Msg<WM_KEYUP>    & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnVoicePlay   (BEHAVIOR_EVENT_PARAMS * params);
	void OnVoiceRecord (BEHAVIOR_EVENT_PARAMS * params);
	void OnVoiceStop   (BEHAVIOR_EVENT_PARAMS * params);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};