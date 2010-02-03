#pragma once
#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "window.h"

class NoteListView : public Window
{
public:
	NoteListView(HINSTANCE hInstance, int nCmdShow);
// window message handlers
private:
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
// HTMLayout message handlers
private:
	static LRESULT CALLBACK HTMLayoutNotifyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam);
	LRESULT OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab);
	ATOM RegisterClass(LPTSTR szWindowClass);
	BOOL GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb);
// data
	HINSTANCE      instance_;       // current instance
	HWND           g_hWndMenuBar; // menu bar handle
	SHACTIVATEINFO s_sai;
};
