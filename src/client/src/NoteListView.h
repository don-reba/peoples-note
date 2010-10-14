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

	enum SearchButtonState
	{
		SearchButtonSearch,
		SearchButtonClear,
	};

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

	element clickTarget;
	element noteList;
	element listScroll;
	element listSlider;
	element searchBox;
	element status;

	Guid selectedNotebookGuid;

	WndMsgPtr         lButtonDown;
	int               lButtonDownY;
	int               startScrollPos;
	double            dragSpeed;
	int               startTime;
	SearchButtonState searchButtonState;
	State             state;

	signal SignalAbout;
	signal SignalClearSearch;
	signal SignalImport;
	signal SignalNewInkNote;
	signal SignalNewNote;
	signal SignalNewPhotoNote;
	signal SignalNotebookSelected;
	signal SignalOpenNote;
	signal SignalPageDown;
	signal SignalPageUp;
	signal SignalProfile;
	signal SignalSearch;
	signal SignalSearchChanged;
	signal SignalSignIn;
	signal SignalSync;

// interface

public:

	NoteListView
		( HINSTANCE   instance
		, bool        highRes
		, IAnimator & animator
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

	virtual void ClearNotes();

	virtual void ConnectAbout(slot_type OnAbout);

	virtual void ConnectClearSearch(slot_type OnClearSearch);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectNewInkNote(slot_type OnNewInkNote);

	virtual void ConnectNewNote(slot_type OnNewNote);

	virtual void ConnectNewPhotoNote(slot_type OnNewPhotoNote);

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectPageDown(slot_type OnPageDown);

	virtual void ConnectPageUp(slot_type OnPageUp);

	virtual void ConnectProfile(slot_type OnProfile);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSearchChanged(slot_type OnSearchChanged);

	virtual void ConnectSignIn(slot_type OnSignin);

	virtual void ConnectSync(slot_type OnSync);

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void GetThumbSize(SIZE & size);

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

	virtual void SetNotebookMenu(const std::wstring & html);

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetProgress(double fraction);

	virtual void SetSearchButtonToClear();

	virtual void SetSearchButtonToSearch();

	virtual void SetSearchText(const std::wstring & text);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetStatusText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & guid);

// utility functions

private:

	void AnimateScroll(DWORD time);

	element GetChild(element parent, element descendant);

	int GetNoteListScrollPos();

	static bool IsChild(element child, element parent);

	void OnSearch();

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

	void OnMenuAbout     (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuExit      (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuImport    (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuNotebook  (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuProfile   (BEHAVIOR_EVENT_PARAMS * params);
	void OnMenuSignIn    (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewInk        (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewPhoto      (BEHAVIOR_EVENT_PARAMS * params);
	void OnNewText       (BEHAVIOR_EVENT_PARAMS * params);
	void OnNote          (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageDown      (BEHAVIOR_EVENT_PARAMS * params);
	void OnPageUp        (BEHAVIOR_EVENT_PARAMS * params);
	void OnSearch        (BEHAVIOR_EVENT_PARAMS * params);
	void OnSearchChanged (BEHAVIOR_EVENT_PARAMS * params);
	void OnStatus        (BEHAVIOR_EVENT_PARAMS * params);
	void OnSync          (BEHAVIOR_EVENT_PARAMS * params);
};
