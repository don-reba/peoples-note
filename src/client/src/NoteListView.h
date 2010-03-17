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

	element   noteList;
	WndMsgPtr lButtonDown;
	int       lButtonDownY;
	int       startScrollPos;
	double    dragSpeed;
	int       startTime;
	State     state;

	signal SignalImport;
	signal SignalOpenNote;
	signal SignalSearch;

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

	virtual void AddNote(std::wstring noteHtml, std::wstring value);

	virtual void AddNotebook(std::wstring notebook);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

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

	void OnMenuExit();
	void OnMenuImport();
	void OnNote();
	void OnSearch();
};
