#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "window.h"
#include "INoteListView.h"

class NoteListView : public Window, public INoteListView
{
private:

	static const int NotebooksMenuIndex = 2;

	SHACTIVATEINFO activateInfo;
	int            cmdShow;
	HINSTANCE      instance;
	HWND           menuBar;
	HMENU          notebooksMenu;

	signal SignalCreated;
	signal SignalSearch;

public:

	NoteListView(HINSTANCE hInstance, int cmdShow);

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void Create();

	virtual void ClearNotes();

	virtual void AddNote(std::wstring noteHtml);

	virtual void UpdateNotes();

	virtual void ClearNotebooks();

	virtual void AddNotebook(std::wstring notebook);

	virtual void UpdateNotebooks();

	virtual std::wstring GetSearchString();

private:

	// window message handlers

	void OnActivate       (Msg<WM_ACTIVATE>        & msg);
	void OnBehaviorNotify (Msg<WM_BEHAVIOR_NOTIFY> & msg);
	void OnCommand        (Msg<WM_COMMAND>         & msg);
	void OnCreate         (Msg<WM_CREATE>          & msg);
	void OnDestroy        (Msg<WM_DESTROY>         & msg);
	void OnSettingChange  (Msg<WM_SETTINGCHANGE>   & msg);

	void ProcessMessage(WndMsg &msg);

private:

	// HTMLayout message handlers

	bool ProcessHtmLayout(WndMsg &msg);

	LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab);

	LRESULT ProcessHTMLayoutNotify
		( UINT   uMsg
		, WPARAM wParam
		, LPARAM lParam
		);

	static LRESULT CALLBACK HTMLayoutNotifyHandler
		( UINT   uMsg
		, WPARAM wParam
		, LPARAM lParam
		, LPVOID vParam
		);

private:

	void CreateMenuBar();

	ATOM RegisterClass(std::wstring wndClass);

	BOOL GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb);

	void ResizeForMenuBar();
};
