#include "stdafx.h"
#include "AudioPlayerView.h"

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

AudioPlayerView::AudioPlayerView
	( HINSTANCE         instance
	, bool              highRes
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"audio-player.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void AudioPlayerView::Create(HWND parent)
{
	this->parent = parent;
	RegisterClass(LoadStringResource(IDC_AUDIO_PLAYER));
}

void AudioPlayerView::RegisterEventHandlers()
{
	ConnectBehavior("#play", BUTTON_CLICK, &AudioPlayerView::OnVoicePlay);
	ConnectBehavior("#stop", BUTTON_CLICK, &AudioPlayerView::OnVoiceStop);
}

//--------------------------------
// IAudioPlayerView implementation
//--------------------------------

void AudioPlayerView::Hide()
{
	if (hwnd_)
	{
		CloseWindow(hwnd_);
		hwnd_ = NULL;
	}
}

void AudioPlayerView::SetFileName(wstring & name)
{
	element(FindFirstElement("#name")).set_text(name.c_str());
}

void AudioPlayerView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_AUDIO_PLAYER);

	DWORD windowStyle   (WS_POPUP);
	DWORD windowExStyle (WS_EX_CAPTIONOKBTN);

	hwnd_ = ::CreateWindowEx
		( windowExStyle    // dwExStyle
		, wndClass.c_str() // lpClassName
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
	menuBarInfo.dwFlags    = SHCMBF_HIDESIPBUTTON;
	menuBarInfo.nToolBarId = IDR_OK_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);

	ResizeWindow();

	ShowWindow(hwnd_, SW_SHOW);

	SignalShow();
}

void AudioPlayerView::OnVoicePlay(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalPlay();
}

void AudioPlayerView::OnVoiceStop(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalStop();
}

//------------------------
// window message handlers
//------------------------

void AudioPlayerView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void AudioPlayerView::OnClose(Msg<WM_CLOSE> & msg)
{
	SignalHide();
}

void AudioPlayerView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDOK:   SignalCancel(); break;
	case IDM_OK: SignalCancel(); break;
	}
}

void AudioPlayerView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalCancel();
		msg.handled_ = true;
	}
}

void AudioPlayerView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&AudioPlayerView::OnActivate,
		&AudioPlayerView::OnClose,
		&AudioPlayerView::OnCommand,
		&AudioPlayerView::OnKeyUp,
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

ATOM AudioPlayerView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &AudioPlayerView::WndProc<AudioPlayerView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void AudioPlayerView::ResizeWindow()
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
