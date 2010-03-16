#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "HTMLayoutWindow.h"
#include "INoteListView.h"
#include "NoteView.h"

class NoteListView : public HTMLayoutWindow, public INoteListView
{
private:

	static const int NotebooksMenuIndex = 2;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;

	signal SignalImport;
	signal SignalOpenNote;
	signal SignalSearch;

// interface

public:

	NoteListView
		( HINSTANCE  instance
		, int        cmdShow
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

	ATOM RegisterClass(std::wstring wndClass);

	void ResetUiSetup();

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
