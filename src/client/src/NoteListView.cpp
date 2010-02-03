#include "stdafx.h"

#include "htmlayout.h"
#include "NoteListView.h"
#include "resourceppc.h"
#include "Tools.h"

#include <string>
#include <vector>

using namespace Tools;


#define MAX_LOADSTRING 100


NoteListView::NoteListView(HINSTANCE hInstance, int nCmdShow)
	: instance_ (hInstance)
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	// SHInitExtraControls should be called once during your application's initialization to initialize any
	// of the device specific controls such as CAPEDIT and SIPPREF.
	SHInitExtraControls();
#endif // WIN32_PLATFORM_PSPC || WIN32_PLATFORM_WFSP

	TCHAR szTitle[MAX_LOADSTRING];       // title bar text
	TCHAR szWindowClass[MAX_LOADSTRING]; // main window class name
	LoadString(hInstance, IDS_APP_TITLE, szTitle,       MAX_LOADSTRING); 
	LoadString(hInstance, IDC_CLIENT,    szWindowClass, MAX_LOADSTRING);

	if (!RegisterClass(szWindowClass))
		throw std::exception("Class could not be registered.");

	hwnd_ = CreateWindow
		( szWindowClass // lpClassName
		, szTitle       // lpWindowName
		, WS_VISIBLE    // dwStyle
		, CW_USEDEFAULT // x
		, CW_USEDEFAULT // y
		, CW_USEDEFAULT // nWidth
		, CW_USEDEFAULT // nHeight
		, NULL          // hWndParent
		, NULL          // hMenu
		, hInstance     // hInstance
		, this          // lpParam
		);

	if (!hwnd_)
	{
		throw std::exception("Window creation failed.");
	}

#ifdef WIN32_PLATFORM_PSPC
	// When the main window is created using CW_USEDEFAULT the height of the menubar (if one
	// is created is not taken into account). So we resize the window after creating it
	// if a menubar is present
	if (g_hWndMenuBar)
	{
		RECT rc;
		RECT rcMenuBar;

		GetWindowRect(hwnd_, &rc);
		GetWindowRect(g_hWndMenuBar, &rcMenuBar);
		rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);

		MoveWindow(hwnd_, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
	}
#endif // WIN32_PLATFORM_PSPC

	ShowWindow(hwnd_, nCmdShow);
	UpdateWindow(hwnd_);
}

bool NoteListView::SwitchToPreviousInstance(HINSTANCE instance)
{
	std::wstring title       = LoadStringResource(instance, IDS_APP_TITLE);
	std::wstring windowClass = LoadStringResource(instance, IDC_CLIENT);
	HWND hwnd = FindWindow(windowClass.c_str(), title.c_str());	
	if (hwnd)
	{
		HWND activeOwnedWindow = (HWND)((ULONG)hwnd | 1);
		SetForegroundWindow(activeOwnedWindow);
		return true;
	}
	return false;
}

ATOM NoteListView::RegisterClass(LPTSTR szWindowClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &NoteListView::WndProc<NoteListView>;
	wc.hInstance     = instance_;
	wc.hIcon         = LoadIcon(instance_, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = szWindowClass;
	return ::RegisterClass(&wc);
}

BOOL NoteListView::GetHtmlResource(int id, /*out*/PBYTE& pb, /*out*/DWORD& cb)
{
	const wchar_t * const RT_HTML = reinterpret_cast<wchar_t * const>(23);

	// Find specified resource and check if ok
	HRSRC hrsrc = ::FindResource(instance_, MAKEINTRESOURCE(id), RT_HTML);
	if(!hrsrc)
		return false;

	// Load specified resource and check if ok
	HGLOBAL hgres = ::LoadResource(instance_, hrsrc);
	if(!hgres) return FALSE;

	// Retrieve resource data and check if ok
	pb = (PBYTE)::LockResource(hgres); if (!pb) return FALSE;
	cb = ::SizeofResource(instance_, hrsrc); if (!cb) return FALSE;

	return TRUE;
}

// HTMLayout specific notification handler.
LRESULT CALLBACK NoteListView::HTMLayoutNotifyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LPVOID vParam)
{
	NoteListView * noteListView = reinterpret_cast<NoteListView*>(vParam);

	// all HTMLayout notification are comming here.
	NMHDR*  phdr = (NMHDR*)lParam;

	switch(phdr->code)
	{
	case HLN_CREATE_CONTROL:    break; //return OnCreateControl((LPNMHL_CREATE_CONTROL) lParam);
	case HLN_CONTROL_CREATED:   break; //return OnControlCreated((LPNMHL_CREATE_CONTROL) lParam);
	case HLN_DESTROY_CONTROL:   break; //return OnDestroyControl((LPNMHL_DESTROY_CONTROL) lParam);
	case HLN_LOAD_DATA:         break; //return OnLoadData((LPNMHL_LOAD_DATA) lParam);
	case HLN_DATA_LOADED:       break; //return OnDataLoaded((LPNMHL_DATA_LOADED)lParam);
	case HLN_DOCUMENT_COMPLETE: break; //return OnDocumentComplete();
	case HLN_ATTACH_BEHAVIOR:   return noteListView->OnAttachBehavior((LPNMHL_ATTACH_BEHAVIOR)lParam );
	}
	return 0;
}

LRESULT NoteListView::OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab )
{
	// attach custom behaviors
	htmlayout::event_handler *pb = htmlayout::behavior::find(lpab->behaviorName, lpab->element);
	//assert(pb);
	if(pb)
	{
		lpab->elementTag  = pb;
		lpab->elementProc = htmlayout::behavior::element_proc;
		lpab->elementEvents = pb->subscribed_to;
	}
	return 0;
}

//-----------------
// message handlers
//-----------------

void NoteListView::OnActivate(Msg<WM_ACTIVATE> &msg)
{
	SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &s_sai, FALSE);
	msg.result_  = 0;
	msg.handled_ = true;
}

void NoteListView::OnBehaviorNotify(Msg<WM_BEHAVIOR_NOTIFY> &msg)
{
	msg.handled_ = true;
}

void NoteListView::OnCommand(Msg<WM_COMMAND> &msg)
{
	switch (msg.GetCtrlId())
	{
	case IDM_MENU_ABOUT:
		// TODO
		break;
	case IDM_MENU_EXIT:
		SendMessage(hwnd_, WM_CLOSE, 0, 0);
		msg.handled_ = true;
		break;
	}
}

void NoteListView::OnCreate(Msg<WM_CREATE> &msg)
{
#ifdef SHELL_AYGSHELL
	SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwnd_;
	mbi.nToolBarId = IDR_MENU;
	mbi.hInstRes   = instance_;

	if (!SHCreateMenuBar(&mbi)) 
	{
		g_hWndMenuBar = NULL;
	}
	else
	{
		g_hWndMenuBar = mbi.hwndMB;
	}

	// Initialize the shell activate info structure
	memset(&s_sai, 0, sizeof (s_sai));
	s_sai.cbSize = sizeof (s_sai);
#endif // SHELL_AYGSHELL
	HTMLayoutSetCallback(hwnd_, &HTMLayoutNotifyHandler, this);

	PBYTE pb; DWORD cb;
	if(GetHtmlResource(IDH_MAIN, pb, cb))
	{
		HTMLayoutLoadHtml(hwnd_, pb, cb);
	}

	msg.handled_ = true;
}

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
#ifdef SHELL_AYGSHELL
	CommandBar_Destroy(g_hWndMenuBar);
#endif // SHELL_AYGSHELL
	PostQuitMessage(0);
}

void NoteListView::OnSettingChange(Msg<WM_SETTINGCHANGE> &msg)
{
	SHHandleWMSettingChange(hwnd_, msg.wprm_, msg.lprm_, &s_sai);
	msg.handled_ = true;
}

bool NoteListView::ProcessHtmLayout(WndMsg &msg)
{
	LRESULT result;
	BOOL    handled;
	result = HTMLayoutProcND(hwnd_, msg.id_, msg.wprm_, msg.lprm_, &handled);
	if (handled)
	{
		msg.result_  = result;
		msg.handled_ = true;
	}
	return handled != FALSE;
}

void NoteListView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteListView::OnActivate,
		&NoteListView::OnBehaviorNotify,
		&NoteListView::OnCommand,
		&NoteListView::OnCreate,
		&NoteListView::OnDestroy,
		&NoteListView::OnSettingChange
	};
	if (!ProcessHtmLayout(msg) && !Handler::Call(mmp, this, msg))
		__super::ProcessMessage(msg);
}
