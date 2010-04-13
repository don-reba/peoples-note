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

	IAnimator::Connection animation;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;

	element noteList;
	element notebookList;

	bool isSignedIn;

	WndMsgPtr lButtonDown;
	int       lButtonDownY;
	int       startScrollPos;
	double    dragSpeed;
	int       startTime;
	State     state;

	signal SignalImport;
	signal SignalOpenNote;
	signal SignalSearch;
	signal SignalSignIn;

	DataSignal  SignalLoadThumbnail;

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

	virtual void AddNote(std::wstring html, std::wstring value);

	virtual void AddNotebook(std::wstring html);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectLoadThumbnail(DataSlot OnLoadThumbnail);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSignIn(slot_type OnSignin);

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void SignIn();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();

// utility functions

private:

	void AnimateScroll(DWORD time);

	int GetNoteListScrollPos();

	static bool IsChild(element child, element parent);

	ATOM RegisterClass(std::wstring wndClass);

	void ResetUiSetup();

	void SetNoteListScrollPos(int pos);

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

	virtual BOOL OnLoadData(NMHL_LOAD_DATA * params);

	void OnMenuExit();
	void OnMenuImport();
	void OnMenuSignIn();
	void OnNote();
	void OnSearch();
};
