#include "stdafx.h"
#include "AboutView.h"

#include "crackers.h"
#include "IHtmlDataLoader.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

//----------
// interface
//----------

AboutView::AboutView(HINSTANCE instance, bool highRes)
	: instance (instance)
	, HTMLayoutWindow(L"about.htm", highRes)
{
}

void AboutView::Create(HWND parent)
{
	this->parent = parent;

	wstring wndClass = LoadStringResource(IDC_ABOUT_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");
}

//--------------------------
// IAboutView implementation
//--------------------------

void AboutView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void AboutView::Hide()
{
	::CloseWindow(hwnd_);
}

void AboutView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_ABOUT_VIEW);

	DWORD windowStyle(WS_VISIBLE);

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, wndTitle.c_str() // lpWindowName
		, windowStyle      // dwStyle
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
}

//------------------------
// window message handlers
//------------------------

void AboutView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalClose();
		msg.handled_ = true;
	}
}

void AboutView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&AboutView::OnKeyUp,
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

ATOM AboutView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &AboutView::WndProc<AboutView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}
