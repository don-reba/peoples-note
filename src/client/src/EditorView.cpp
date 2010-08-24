#include "stdafx.h"
#include "EditorView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

EditorView::EditorView(HINSTANCE instance)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"note-edit.htm")
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
}

//---------------------------
// IEditorView implementation
//---------------------------

void EditorView::ConnectAccept(slot_type OnAccept)
{
	SignalAccept.connect(OnAccept);
}

void EditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void EditorView::GetBody(std::wstring & html)
{
	html = element(FindFirstElement("#body")).get_value().to_string();
}

void EditorView::GetNote(Note & note)
{
	note = this->note;
}

void EditorView::GetTitle(std::wstring & text)
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
	case IDM_OK:     SignalAccept(); break;
	case IDM_CANCEL: SignalCancel(); break;
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

void EditorView::OnInput(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	if (e.get_value().get(false))
		e.set_attribute("checked", L"");
	else
		e.remove_attribute("checked");
}

//------------------
// utility functions
//------------------

ATOM EditorView::RegisterClass(const std::wstring & wndClass)
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
	SIPINFO sipInfo = { sizeof(sipInfo) };
	if (::SipGetInfo(&sipInfo))
	{
		if ((sipInfo.fdwFlags & SIPF_ON) == SIPF_ON)
		{
			RECT & rect(sipInfo.rcVisibleDesktop);
			::MoveWindow
				( hwnd_                  // hwnd
				, rect.left              // x
				, rect.top               // y
				, rect.right - rect.left // nWidth
				, rect.bottom - rect.top // nHeight
				, TRUE				     // bRepaint
				);
		}
		else
		{
			if (menuBar)
			{
				RECT & desktopRect(sipInfo.rcVisibleDesktop);
				int desktopWidth  (desktopRect.right  - desktopRect.left);
				int desktopHeight (desktopRect.bottom - desktopRect.top);

				RECT menuBarRect;
				::GetWindowRect(menuBar, &menuBarRect);
				int menuBarHeight(menuBarRect.bottom - menuBarRect.top);

				int windowWidth  (desktopWidth);
				int windowHeight (desktopHeight - menuBarHeight);

				::MoveWindow
					( hwnd_            // hwnd
					, desktopRect.left // x
					, desktopRect.top  // y
					, windowWidth      // nWidth
					, windowHeight     // nHeight
					, TRUE			   // bRepaint
					);
			}

		}
	}
}
