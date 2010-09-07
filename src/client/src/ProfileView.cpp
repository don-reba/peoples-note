#include "stdafx.h"
#include "ProfileView.h"

#include "crackers.h"
#include "IHtmlDataLoader.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

//----------
// interface
//----------

ProfileView::ProfileView(HINSTANCE instance)
	: instance (instance)
	, HTMLayoutWindow(L"profile.htm")
{
}

void ProfileView::Create(HWND parent)
{
	this->parent = parent;

	wstring wndClass = LoadStringResource(IDC_PROFILE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");
}

//--------------------------
// IProfileView implementation
//--------------------------

void ProfileView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void ProfileView::Hide()
{
	::CloseWindow(hwnd_);
}

void ProfileView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_PROFILE_VIEW);

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

void ProfileView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalClose();
		msg.handled_ = true;
	}
}

void ProfileView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&ProfileView::OnKeyUp,
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

ATOM ProfileView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &ProfileView::WndProc<ProfileView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}
