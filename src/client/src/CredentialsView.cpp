#include "stdafx.h"
#include "CredentialsView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

CredentialsView::CredentialsView(HINSTANCE instance)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"signin-view.htm")
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void CredentialsView::Create(HWND parent)
{
	this->parent = parent;

	wstring wndClass = LoadStringResource(IDC_CREDENTIALS_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");
}

void CredentialsView::RegisterEventHandlers()
{
	ConnectBehavior("#ok",     BUTTON_CLICK, &CredentialsView::OnOk);
	ConnectBehavior("#cancel", BUTTON_CLICK, &CredentialsView::OnCancel);
}

//--------------------------------
// ICredentialsView implementation
//--------------------------------

void CredentialsView::Close()
{
	CloseWindow(hwnd_);
}

void CredentialsView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void CredentialsView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void CredentialsView::ConnectOk(slot_type OnOk)
{
	SignalOk.connect(OnOk);
}

wstring CredentialsView::GetPassword() const
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#password");
	if (!e)
		throw std::exception("#password not found.");
	return e.text().c_str();
}

wstring CredentialsView::GetUsername() const
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#username");
	if (!e)
		throw std::exception("#username not found.");
	return e.text().c_str();
}

void CredentialsView::Open()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_CREDENTIALS_VIEW);

	DWORD windowStyle(WS_POPUP);

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

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.nToolBarId = IDR_SIGNIN_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);

	ResizeWindow();

	ShowWindow(hwnd_, SW_SHOW);
}

void CredentialsView::SetMessage(const std::wstring & message)
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#message");
	if (!e)
		throw std::exception("#message not found.");
	e.set_text(message.c_str());
}

void CredentialsView::SetPassword(const std::wstring & password)
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#password");
	if (!e)
		throw std::exception("#password not found.");
	e.set_text(password.c_str());
}

void CredentialsView::SetUsername(const std::wstring & username)
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#username");
	if (!e)
		throw std::exception("#username not found.");
	e.set_text(username.c_str());
}

//------------------
// utility functions
//------------------

void CredentialsView::CopyParentSize()
{	
	RECT rect;
	::GetWindowRect(parent, &rect);
	::MoveWindow
		( hwnd_
		, rect.left
		, rect.top
		, rect.right - rect.left
		, rect.bottom - rect.top
		, TRUE
		);
}

ATOM CredentialsView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &CredentialsView::WndProc<CredentialsView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void CredentialsView::ResizeWindow()
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
			HWND menuBar(::SHFindMenuBar(hwnd_));
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

//------------------------
// window message handlers
//------------------------

void CredentialsView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SipRegisterNotification(hwnd_);
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void CredentialsView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDM_SIGNIN: SignalOk();     break;
	case IDM_CANCEL: SignalCancel(); break;
	}
}

void CredentialsView::OnSettingChange(Msg<WM_SETTINGCHANGE> & msg)
{
	if (msg.Flag() == SPI_SETSIPINFO)
	{
		ResizeWindow();
		msg.handled_ = true;
	}
}

void CredentialsView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&CredentialsView::OnActivate,
		&CredentialsView::OnCommand,
		&CredentialsView::OnSettingChange,
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

void CredentialsView::OnCancel(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalCancel();
}

void CredentialsView::OnOk(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalOk();
}
