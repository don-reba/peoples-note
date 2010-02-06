#include "stdafx.h"

#include "htmlayout.h"
#include "NoteListView.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

NoteListView::NoteListView(HINSTANCE instance, int cmdShow)
	: cmdShow  (cmdShow)
	, instance (instance)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void NoteListView::Create()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_CLIENT);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, wndTitle.c_str() // lpWindowName
		, WS_VISIBLE       // dwStyle
		, CW_USEDEFAULT    // x
		, CW_USEDEFAULT    // y
		, CW_USEDEFAULT    // nWidth
		, CW_USEDEFAULT    // nHeight
		, NULL             // hWndParent
		, NULL             // hMenu
		, instance         // hInstance
		, this             // lpParam
		);
	if (!hwnd_)
		throw std::exception("Window creation failed.");

	ResizeForMenuBar();

	::ShowWindow(hwnd_, cmdShow);
	::UpdateWindow(hwnd_);
}

void NoteListView::ClearNotes()
{
	// TODO: implement
}

void NoteListView::AddNote(wstring note)
{
	// TODO: implement
}

void NoteListView::UpdateNotes()
{
	// TODO: implement
}

void NoteListView::ClearNotebooks()
{
	// TODO: implement
}

void NoteListView::AddNotebook(wstring notebook)
{
	// TODO: implement
}

void NoteListView::UpdateNotebooks()
{
	// TODO: implement
}

//-----------------
// message handlers
//-----------------

void NoteListView::OnActivate(Msg<WM_ACTIVATE> &msg)
{
	SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, FALSE);
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
		// TODO: implement About dialog
		break;
	case IDM_MENU_EXIT:
		SendMessage(hwnd_, WM_CLOSE, 0, 0);
		msg.handled_ = true;
		break;
	}
}

void NoteListView::OnCreate(Msg<WM_CREATE> &msg)
{
	CreateMenuBar();

	HTMLayoutSetCallback(hwnd_, &HTMLayoutNotifyHandler, this);

	HtmlResource resource = Tools::LoadHtmlResource(IDH_MAIN);
	HTMLayoutLoadHtml(hwnd_, resource.data, resource.size);

	msg.handled_ = true;
}

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
	CommandBar_Destroy(menuBar);
	PostQuitMessage(0);
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

void NoteListView::OnSettingChange(Msg<WM_SETTINGCHANGE> &msg)
{
	SHHandleWMSettingChange(hwnd_, msg.wprm_, msg.lprm_, &activateInfo);
	msg.handled_ = true;
}

//---------------------------
// HTMLayout message handlers
//---------------------------

LRESULT NoteListView::OnAttachBehavior(LPNMHL_ATTACH_BEHAVIOR lpab )
{
	// attach custom behaviors
	htmlayout::event_handler * handler = htmlayout::behavior::find
		( lpab->behaviorName
		, lpab->element
		);
	if(handler)
	{
		lpab->elementTag    = handler;
		lpab->elementProc   = htmlayout::behavior::element_proc;
		lpab->elementEvents = handler->subscribed_to;
	}
	return 0;
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

//-------------------
// Internal functions
//-------------------

ATOM NoteListView::RegisterClass(wstring wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &NoteListView::WndProc<NoteListView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void NoteListView::CreateMenuBar()
{
	SHMENUBARINFO mbi = { sizeof(SHMENUBARINFO) };
	mbi.hwndParent = hwnd_;
	mbi.nToolBarId = IDR_MENU;
	mbi.hInstRes   = instance;
	if (!SHCreateMenuBar(&mbi)) 
		menuBar = NULL;
	else
		menuBar = mbi.hwndMB;
}

void NoteListView::ResizeForMenuBar()
{
	if (menuBar)
	{
		RECT windowRect;
		RECT menuBarRect;

		::GetWindowRect(hwnd_,   &windowRect);
		::GetWindowRect(menuBar, &menuBarRect);
		windowRect.bottom -= (menuBarRect.bottom - menuBarRect.top);

		int  x       = windowRect.left;
		int  y       = windowRect.top;
		int  width   = windowRect.right  - windowRect.left;
		int  height  = windowRect.bottom - windowRect.top;
		BOOL repaint = FALSE;

		::MoveWindow(hwnd_, x, y, width, height, repaint);
	}
}
