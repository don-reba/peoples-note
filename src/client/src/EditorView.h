#pragma once
#include "IEditorView.h"

#include "HTMLayoutWindow.h"
#include "Note.h"

class EditorView : public HTMLayoutWindow, public IEditorView
{
// data

private:

	HWND           parent;
	HWND           menuBar;
	HINSTANCE      instance;
	SHACTIVATEINFO activateInfo;

	Note note;

	signal SignalAccept;
	signal SignalCancel;

// interface

public:

	EditorView(HINSTANCE instance, bool highRes);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// IEditorView implementation

public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void GetBody(std::wstring & html);

	virtual void GetNote(Note & note);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual void SetNote
		( const Note         & note
		, const std::wstring & bodyHtml
		);

	virtual void Show();

// window message handlers

private:

	void OnActivate      (Msg<WM_ACTIVATE>      & msg);
	void OnCommand       (Msg<WM_COMMAND>       & msg);
	void OnSettingChange (Msg<WM_SETTINGCHANGE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnCheck (BEHAVIOR_EVENT_PARAMS * params);
	void OnInput (BEHAVIOR_EVENT_PARAMS * params);

// utility functions

private:

	ATOM RegisterClass(const std::wstring & wndClass);

	void ResizeWindow();
};
