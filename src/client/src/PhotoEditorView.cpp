#include "stdafx.h"
#include "PhotoEditorView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

#include <fstream>

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

PhotoEditorView::PhotoEditorView
	( HINSTANCE         instance
	, bool              highRes
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"photo-edit.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void PhotoEditorView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_PHOTO_EDIT)))
		throw std::exception("Class could not be registered.");
}

//--------------------------------
// IPhotoEditorView implementation
//--------------------------------

int PhotoEditorView::CapturePhoto(int quality, int width, int height, wstring & path)
{
	SHCAMERACAPTURE settings = { sizeof(settings) };
	settings.hwndOwner         = hwnd_;
	settings.pszTitle          = L"Photo note";
	settings.StillQuality      = static_cast<CAMERACAPTURE_STILLQUALITY>(quality);
	settings.nResolutionWidth  = width;
	settings.nResolutionHeight = height;
	settings.Mode              = CAMERACAPTURE_MODE_STILL;

	HRESULT result(::SHCameraCapture(&settings));
	if (result == S_OK)
		path = settings.szFile;
	return result;
}

PhotoQuality PhotoEditorView::GetQuality()
{
	return static_cast<PhotoQuality>
		( element(FindFirstElement("#quality")).get_value().get(0)
		);
}

PhotoResolution PhotoEditorView::GetResolution()
{
	return static_cast<PhotoResolution>
		( element(FindFirstElement("#resolution")).get_value().get(0)
		);
}

void PhotoEditorView::Hide()
{
	CloseWindow(hwnd_);
	hwnd_ = NULL;
}

std::wstring PhotoEditorView::GetTitle()
{
	return element(FindFirstElement("#title")).text().c_str();
}

void PhotoEditorView::SetQuality(PhotoQuality quality)
{
	element(FindFirstElement("#quality")).set_value(quality);
}

void PhotoEditorView::SetMessage(const wstring & message)
{
	element(FindFirstElement("#message")).set_text(message.c_str());
}

void PhotoEditorView::SetResolution(PhotoResolution resolution)
{
	element(FindFirstElement("#resolution")).set_value(resolution);
}

void PhotoEditorView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_PHOTO_EDIT);

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
	menuBar = menuBarInfo.hwndMB;

	ResizeWindow();

	ShowWindow(hwnd_, SW_SHOW);
}

//------------------------
// window message handlers
//------------------------

void PhotoEditorView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void PhotoEditorView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDM_OK:     SignalOk();     break;
	case IDM_CANCEL: SignalCancel(); break;
	case IDCANCEL:   SignalCancel(); break;
	}
}

void PhotoEditorView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalCancel();
		msg.handled_ = true;
	}
}

void PhotoEditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&PhotoEditorView::OnActivate,
		&PhotoEditorView::OnCommand,
		&PhotoEditorView::OnKeyUp,
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

//------------------
// utility functions
//------------------

ATOM PhotoEditorView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &PhotoEditorView::WndProc<PhotoEditorView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void PhotoEditorView::ResizeWindow()
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
