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

PhotoEditorView::PhotoEditorView(HINSTANCE instance, bool highRes)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"photo-edit.htm", highRes)
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

void PhotoEditorView::ConnectCapture(slot_type OnCapture)
{
	SignalCapture.connect(OnCapture);
}

void PhotoEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void PhotoEditorView::ConnectOk(slot_type OnOk)
{
	SignalOk.connect(OnOk);
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

void PhotoEditorView::InitiateCapture()
{
	PhotoQuality    quality    (GetQuality());
	PhotoResolution resolution (GetResolution());

	SHCAMERACAPTURE settings = { sizeof(settings) };
	settings.hwndOwner         = hwnd_;
	settings.pszTitle          = L"Photo note";
	settings.StillQuality      = GetPhotoQuality(quality);
	settings.nResolutionWidth  = GetPhotoWidth(resolution);
	settings.nResolutionHeight = GetPhotoHeight(resolution);
	settings.Mode              = CAMERACAPTURE_MODE_STILL;
	if (S_OK == ::SHCameraCapture(&settings))
	{
		photoPath = settings.szFile;
		SignalCapture();
	}
}

wstring PhotoEditorView::GetImagePath()
{
	return photoPath;
}

std::wstring PhotoEditorView::GetTitle()
{
	return element(FindFirstElement("#title")).text().c_str();
}

void PhotoEditorView::SetQuality(PhotoQuality quality)
{
	element(FindFirstElement("#quality")).set_value(quality);
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

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.nToolBarId = IDR_EDITOR_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);

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
	}
}

void PhotoEditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&PhotoEditorView::OnActivate,
		&PhotoEditorView::OnCommand,
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

DWORD PhotoEditorView::GetPhotoHeight(PhotoResolution resolution)
{
	switch (resolution)
	{
	case PhotoResolutionQvga: return 240;
	case PhotoResolutionVga:  return 480;
	case PhotoResolution1M:   return 960;
	case PhotoResolution2M:   return 1200;
	case PhotoResolution3M:   return 1536;
	}
	return 480;
}

CAMERACAPTURE_STILLQUALITY PhotoEditorView::GetPhotoQuality(PhotoQuality quality)
{
	switch (quality)
	{
	case PhotoQualityDefault: return CAMERACAPTURE_STILLQUALITY_DEFAULT;
	case PhotoQualityLow:     return CAMERACAPTURE_STILLQUALITY_LOW;
	case PhotoQualityNormal:  return CAMERACAPTURE_STILLQUALITY_NORMAL;
	case PhotoQualityHigh:    return CAMERACAPTURE_STILLQUALITY_HIGH;
	}
	return CAMERACAPTURE_STILLQUALITY_DEFAULT;
}

DWORD PhotoEditorView::GetPhotoWidth(PhotoResolution resolution)
{
	switch (resolution)
	{
	case PhotoResolutionQvga: return 320;
	case PhotoResolutionVga:  return 640;
	case PhotoResolution1M:   return 1280;
	case PhotoResolution2M:   return 1600;
	case PhotoResolution3M:   return 2048;
	}
	return 640;
}

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
