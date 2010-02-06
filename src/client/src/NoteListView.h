#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "window.h"

class NoteListView : public Window
{
private:

	int            cmdShow;
	HINSTANCE      instance;
	HWND           menuBar;
	SHACTIVATEINFO activateInfo;

public:

	NoteListView(HINSTANCE hInstance, int cmdShow);

	virtual void Create();

private:

	// window message handlers
	void OnActivate       (Msg<WM_ACTIVATE>        & msg);
	void OnBehaviorNotify (Msg<WM_BEHAVIOR_NOTIFY> & msg);
	void OnCommand        (Msg<WM_COMMAND>         & msg);
	void OnCreate         (Msg<WM_CREATE>          & msg);
	void OnDestroy        (Msg<WM_DESTROY>         & msg);
	void OnSettingChange  (Msg<WM_SETTINGCHANGE>   & msg);

	bool ProcessHtmLayout(WndMsg &msg);
	void ProcessMessage(WndMsg &msg);

private:

	// HTMLayout message handlers
	LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab);

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
