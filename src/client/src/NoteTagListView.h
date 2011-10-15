#pragma once
#include "INoteTagListView.h"

#include "HTMLayoutWindow.h"
#include "Note.h"
#include "NoteListGestureProcessor.h"
#include "WindowRenderer.h"

class IAnimator;

class NoteTagListView : public HTMLayoutWindow, public INoteTagListView
{
	MacroEvent(Cancel)
	MacroEvent(Ok)
	MacroEvent(TagCleared)
	MacroEvent(TagSelected)

private:

	typedef htmlayout::dom::element element;

private:

	HINSTANCE instance;
	HWND      menuBar;
	HWND      parent;

	NoteListGestureProcessor gestureProcessor;

	Note note;

	int startScrollPos;

	bool isDirty;

	element body;
	element vScroll;
	element vSlider;

	std::wstring activeTag;

// interface

public:

	NoteTagListView
		( HINSTANCE         instance
		, bool              highRes
		, IAnimator       & animator
		, IHtmlDataLoader & htmlDataLoader
		);

	void Create(HWND parent);

	virtual void RegisterEventHandlers();

// INoteTagListView implementation

public:

	virtual void Hide();

	virtual std::wstring GetActiveTag();

	virtual void SetTags
		( const TagList & cleared
		, const TagList & selected
		);

	virtual void Show();

// utility functions

private:

	element CreateClearedTag(const Tag & tag);

	element CreateSelectedTag(const Tag & tag);

	int GetScrollPos();

	ATOM RegisterClass(const std::wstring & wndClass);

	void SetScrollPos(int pos);

	void UpdateScrollbar();

	void UpdateWindowState();

// gesture message handlers

private:

	void OnDelayedMouseDown();
	void OnGestureStart();
	void OnGestureStep();

// window message handlers

private:

	void OnActivate      (Msg<WM_ACTIVATE>      & msg);
	void OnCommand       (Msg<WM_COMMAND>       & msg);
	void OnMouseDown     (Msg<WM_LBUTTONDOWN>   & msg);
	void OnMouseUp       (Msg<WM_LBUTTONUP>     & msg);
	void OnMouseMove     (Msg<WM_MOUSEMOVE>     & msg);
	void OnKeyUp         (Msg<WM_KEYUP>         & msg);
	void OnSettingChange (Msg<WM_SETTINGCHANGE> & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnCleared  (BEHAVIOR_EVENT_PARAMS * params);
	void OnSelected (BEHAVIOR_EVENT_PARAMS * params);
};
