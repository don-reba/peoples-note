#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "HTMLayoutWindow.h"
#include "INoteListView.h"

class NoteListView : public HTMLayoutWindow, public INoteListView
{
private:

	static const int NotebooksMenuIndex = 2;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;
	HWND           menuBar;
	HMENU          notebooksMenu;

	signal SignalImport;
	signal SignalSearch;

// interface

public:

	NoteListView(HINSTANCE hInstance, int cmdShow);

	void Create();

	virtual void AddNote(std::wstring noteHtml);

	virtual void AddNotebook(std::wstring notebook);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual bool GetEnexPath(std::wstring & path);
	
	virtual std::wstring GetSearchString();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();

// utility functions

private:

	void CreateMenuBar();

	ATOM RegisterClass(std::wstring wndClass);

	void ResizeForMenuBar();

// window message handlers

private:

	void OnActivate       (Msg<WM_ACTIVATE>        & msg);
	void OnBehaviorNotify (Msg<WM_BEHAVIOR_NOTIFY> & msg);
	void OnCommand        (Msg<WM_COMMAND>         & msg);
	void OnCreate         (Msg<WM_CREATE>          & msg);
	void OnDestroy        (Msg<WM_DESTROY>         & msg);
	void OnSettingChange  (Msg<WM_SETTINGCHANGE>   & msg);

	virtual void ProcessMessage(WndMsg &msg);

// HTMLayout message handlers

private:

	void OnSearch();
};
