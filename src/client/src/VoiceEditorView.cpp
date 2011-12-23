#include "stdafx.h"
#include "VoiceEditorView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

#include <fstream>
#include <voicectl.h>

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

VoiceEditorView::VoiceEditorView
	( HINSTANCE         instance
	, bool              highRes
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"voice-edit.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void VoiceEditorView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_VOICE_EDIT)))
		throw std::exception("Class could not be registered.");
}

void VoiceEditorView::RegisterEventHandlers()
{
	ConnectBehavior("#play",   BUTTON_CLICK, &VoiceEditorView::OnVoicePlay);
	ConnectBehavior("#record", BUTTON_CLICK, &VoiceEditorView::OnVoiceRecord);
	ConnectBehavior("#stop",   BUTTON_CLICK, &VoiceEditorView::OnVoiceStop);
}

//--------------------------------
// IVoiceEditorView implementation
//--------------------------------

void VoiceEditorView::Hide()
{
	if (hwnd_)
	{
		CloseWindow(hwnd_);
		hwnd_ = NULL;
	}
}

void VoiceEditorView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_VOICE_EDIT);

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

	::SHDoneButton(hwnd_, SHDB_SHOWCANCEL);

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.nToolBarId = IDR_OK_CANCEL_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);

	ResizeWindow();

	ShowWindow(hwnd_, SW_SHOW);
}

void VoiceEditorView::OnVoicePlay(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalPlay();
}

void VoiceEditorView::OnVoiceRecord(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalRecord();
}

void VoiceEditorView::OnVoiceStop(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalStop();
}

//------------------------
// window message handlers
//------------------------

void VoiceEditorView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void VoiceEditorView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDM_CANCEL: SignalCancel(); break;
	case IDCANCEL:   SignalCancel(); break;
	}
}

void VoiceEditorView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalCancel();
		msg.handled_ = true;
	}
}

void VoiceEditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&VoiceEditorView::OnActivate,
		&VoiceEditorView::OnCommand,
		&VoiceEditorView::OnKeyUp,
	};
	try
	{
		if (!Handler::Call(mmp, this, msg))
			__super::ProcessMessage(msg);
	}
	catch (const std::exception & e)
	{
		::NKDbgPrintfW(L"%s\n", ConvertToUnicode(e.what()).c_str());
		throw e;
	}
}

//------------------
// utility functions
//------------------

ATOM VoiceEditorView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &VoiceEditorView::WndProc<VoiceEditorView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void VoiceEditorView::ResizeWindow()
{
	HWND menuBar(::SHFindMenuBar(hwnd_));
	if (menuBar)
	{
		Rect desktopRect;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, FALSE);

		Rect menuBarRect;
		::GetWindowRect(menuBar, &menuBarRect);

		int windowWidth  (desktopRect.GetWidth());
		int windowHeight (desktopRect.GetHeight() - menuBarRect.GetHeight());

		::MoveWindow
			( hwnd_              // hwnd
			, desktopRect.GetX() // x
			, desktopRect.GetY() // y
			, windowWidth        // nWidth
			, windowHeight       // nHeight
			, TRUE			     // bRepaint
			);
	}
	else
	{
		Rect desktopRect;
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, FALSE);

		::MoveWindow
			( hwnd_                   // hwnd
			, desktopRect.GetX()      // x
			, desktopRect.GetY()      // y
			, desktopRect.GetWidth()  // nWidth
			, desktopRect.GetHeight() // nHeight
			, TRUE			          // bRepaint
			);
	}
}
