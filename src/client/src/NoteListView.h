#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "window.h"

class NoteListView : public Window
{
private:

	HINSTANCE      instance;
	HWND           menuBar;
	SHACTIVATEINFO activateInfo;

public:

	NoteListView(HINSTANCE hInstance, int nCmdShow);

private:

	// window message handlers
	void OnActivate       (Msg<WM_ACTIVATE>        &msg);
	void OnBehaviorNotify (Msg<WM_BEHAVIOR_NOTIFY> &msg);
	void OnCommand        (Msg<WM_COMMAND>         &msg);
	void OnCreate         (Msg<WM_CREATE>          &msg);
	void OnDestroy        (Msg<WM_DESTROY>         &msg);
	void OnSettingChange  (Msg<WM_SETTINGCHANGE>   &msg);

private:

	bool ProcessHtmLayout(WndMsg &msg);

protected:

	void ProcessMessage(WndMsg &msg);

private:

	static LRESULT CALLBACK HTMLayoutNotifyHandler
		( UINT   uMsg
		, WPARAM wParam
		, LPARAM lParam
		, LPVOID vParam
		);

private:

	LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab);

	void CreateMenuBar();

	ATOM RegisterClass(std::wstring wndClass);

	BOOL GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb);

	void ResizeForMenuBar();
};
