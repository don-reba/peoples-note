#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "HTMLayoutWindow.h"
#include "IAnimator.h"
#include "INoteListView.h"
#include "NoteView.h"

class NoteListView : public HTMLayoutWindow, public INoteListView
{
private:

	enum State
	{
		StateIdle,
		StateAnimating,
		StateDragging,
	};

	typedef boost::shared_ptr<WndMsg> WndMsgPtr;
	typedef htmlayout::dom::element   element;

private:

	const double acceleration; // px/ms

	IAnimator & animator;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;
	DWORD          sipState;

	element noteList;
	element notebookList;
	element listScroll;
	element listSlider;
	element searchBox;
	element status;

	Guid selectedNotebookGuid;

	WndMsgPtr lButtonDown;
	int       lButtonDownY;
	int       startScrollPos;
	double    dragSpeed;
	int       startTime;
	State     state;

	signal SignalAbout;
	signal SignalImport;
	signal SignalNewNote;
	signal SignalNotebookSelected;
	signal SignalOpenNote;
	signal SignalPageDown;
	signal SignalPageUp;
	signal SignalProfile;
	signal SignalSearch;
	signal SignalSignIn;
	signal SignalSync;

// interface

public:

	NoteListView
		( IAnimator & animator
		, HINSTANCE   instance
		, int         cmdShow
		);

	void Create();

	virtual void RegisterEventHandlers();

// INoteListView implementation

public:

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		);

	virtual void AddNotebook
		( const std::wstring & html
		, const std::wstring & value
		);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectAbout(slot_type OnAbout);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectNewNote(slot_type OnNewNote);

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectPageDown(slot_type OnPageDown);

	virtual void ConnectPageUp(slot_type OnPageUp);

	virtual void ConnectProfile(slot_type OnProfile);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSignIn(slot_type OnSignin);

	virtual void ConnectSync(slot_type OnSync);

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetProgress(double fraction);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetStatusText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & guid);

// utility functions

private:

	void AnimateScroll(DWORD time);

	element GetChild(element parent, element descendant);

	int GetNoteListScrollPos();

	static bool IsChild(element child, element parent);

	ATOM RegisterClass(std::wstring wndClass);

	void SetNoteListScrollPos(int pos);

	void UpdateScrollbar();

// window message handlers

private:

	void OnActivate       (Msg<WM_ACTIVATE>       & msg);
	void OnCaptureChanged (Msg<WM_CAPTURECHANGED> & msg);
	void OnDestroy        (Msg<WM_DESTROY>        & msg);
	void OnMouseDown      (Msg<WM_LBUTTONDOWN>    & msg);
	void OnMouseUp        (Msg<WM_LBUTTONUP>      & msg);
	void OnMouseMove      (Msg<WM_MOUSEMOVE>      & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	virtual BOOL OnFocus (FOCUS_PARAMS * params);
	virtual BOOL OnKey   (KEY_PARAMS   * params);

	void OnMenuAbout    (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuExit     (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuImport   (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuNotebook (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuProfile  (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuSignIn   (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewText      (BEHAVIOR_EVENT_PARAMS * params);
	void OnNote         (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageDown     (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageUp       (BEHAVIOR_EVENT_PARAMS * params);
	void OnSearch       (BEHAVIOR_EVENT_PARAMS * params);
	void OnSync         (BEHAVIOR_EVENT_PARAMS * params);
};
