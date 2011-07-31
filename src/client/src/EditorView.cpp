#include "stdafx.h"
#include "EditorView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

EditorView::EditorView
	( HINSTANCE         instance
	, bool              highRes
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance        (instance)
	, isMaximized     (false)
	, parent          (NULL)
	, HTMLayoutWindow (L"note-edit.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void EditorView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_NOTE_EDIT)))
		throw std::exception("Class could not be registered.");
}

void EditorView::RegisterEventHandlers()
{
	ConnectBehavior("#check", BUTTON_CLICK, &EditorView::OnCheck);
	ConnectBehavior("#hide",  BUTTON_CLICK, &EditorView::OnHidePanel);
	ConnectBehavior("#show",  BUTTON_CLICK, &EditorView::OnShowPanel);
}

//---------------------------
// IEditorView implementation
//---------------------------

void EditorView::GetBody(wstring & html)
{
	html = element(FindFirstElement("#body")).get_value().to_string();
}

void EditorView::GetNote(Note & note)
{
	note = this->note;
}

void EditorView::GetTitle(wstring & text)
{
	text = element(FindFirstElement("#title")).text();
}

void EditorView::Hide()
{
	CloseWindow(hwnd_);
	hwnd_ = NULL;
}

void EditorView::SetNote
	( const Note    & note
	, const wstring & bodyHtml
	)
{
	this->note = note;

	element title (FindFirstElement("#title"));
	element body  (FindFirstElement("#body"));

	title.set_text(note.name.c_str(), note.name.size());

	DisconnectBehavior("#body input");
	body.set_value(json::value::from_string(bodyHtml));
	ConnectBehavior("#body input", BUTTON_STATE_CHANGED, &EditorView::OnInput);
}

void EditorView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_EDIT);

	DWORD windowStyle(WS_POPUP);

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, wndTitle.c_str() // lpWindowName
		, windowStyle      // dwStyle
		, CW_USEDEFAULT    // x
		, CW_USEDEFAULT    // y
		, CW_USEDEFAULT    // nWidth
		, CW_USEDEFAULT    // nHeight
		, parent           // hWndParent
		, NULL             // hMenu
		, instance         // hInstance
		, this             // lpParam
		);
	if (!hwnd_)
		throw std::exception("Window creation failed.");

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.nToolBarId = IDR_EDITOR_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);
	menuBar = menuBarInfo.hwndMB;

	ResizeWindow();

	ShowWindow(hwnd_, SW_SHOW);
}

//------------------------
// window message handlers
//------------------------

void EditorView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void EditorView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDM_OK:         SignalAccept();     break;
	case IDM_CANCEL:     SignalCancel();     break;
	case ID_DELETE_NOTE: SignalDeleteNote(); break;
	}
}

void EditorView::OnSettingChange(Msg<WM_SETTINGCHANGE> & msg)
{
	if (msg.Flag() == SPI_SETSIPINFO)
	{
		ResizeWindow();
		msg.handled_ = true;
	}
}

void EditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&EditorView::OnActivate,
		&EditorView::OnCommand,
		&EditorView::OnSettingChange,
	};
	try
	{
		if (!Handler::Call(mmp, this, msg))
			__super::ProcessMessage(msg);
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		throw e;
	}
}

//---------------------------
// HTMLayout message handlers
//---------------------------

void EditorView::OnCheck(BEHAVIOR_EVENT_PARAMS * params)
{
	element body(FindFirstElement("#body"));
	body.xcall("insertHtml", json::string(L"<input type=\"checkbox\" />"));
	body.update();
}

void EditorView::OnHidePanel(BEHAVIOR_EVENT_PARAMS * params)
{
	element(FindFirstElement("#panel"))
		.set_style_attribute("display", L"none");
	element(FindFirstElement("#show"))
		.set_style_attribute("display", L"block");
	isMaximized = true;
	ResizeWindow();
}

void EditorView::OnInput(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	if (e.get_value().get(false))
		e.set_attribute("checked", L"");
	else
		e.remove_attribute("checked");
}

void EditorView::OnShowPanel(BEHAVIOR_EVENT_PARAMS * params)
{
	element(FindFirstElement("#show"))
		.set_style_attribute("display", L"none");
	element(FindFirstElement("#panel"))
		.set_style_attribute("display", L"block");
	isMaximized = false;
	ResizeWindow();
}

//------------------
// utility functions
//------------------

ATOM EditorView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &EditorView::WndProc<EditorView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void EditorView::ResizeWindow()
{
	::SHFullScreen
		( hwnd_
		, isMaximized
		? SHFS_HIDETASKBAR
		: SHFS_SHOWTASKBAR
		);

	SIPINFO sipInfo = { sizeof(sipInfo) };
	if (!::SipGetInfo(&sipInfo))
		return;

	Rect rect(sipInfo.rcVisibleDesktop);

	if (isMaximized)
		rect.top = 0;

	if (menuBar && (sipInfo.fdwFlags & SIPF_ON) != SIPF_ON)
	{
		Rect menuBarRect;
		::GetWindowRect(menuBar, &menuBarRect);
		rect.bottom -= menuBarRect.GetHeight();
	}

	::MoveWindow
		( hwnd_            // hwnd
		, rect.GetX()      // x
		, rect.GetY()      // y
		, rect.GetWidth()  // nWidth
		, rect.GetHeight() // nHeight
		, TRUE             // bRepaint
		);
}
