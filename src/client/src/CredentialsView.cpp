#include "stdafx.h"
#include "CredentialsView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

CredentialsView::CredentialsView
	( HINSTANCE         instance
	, bool              highRes
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"signin-view.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void CredentialsView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_CREDENTIALS_VIEW)))
		throw std::exception("Class could not be registered.");
}

//--------------------------------
// ICredentialsView implementation
//--------------------------------

void CredentialsView::Close()
{
	CloseWindow(hwnd_);
	hwnd_ = NULL;
}

wstring CredentialsView::GetShard() const
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#shard");
	if (!e)
		throw std::exception("#shard not found.");
	return static_cast<wstring>(e.get_value().to_string());
}

wstring CredentialsView::GetToken() const
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#token");
	if (!e)
		throw std::exception("#token not found.");
	return static_cast<wstring>(e.get_value().to_string());
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
	if (NULL != hwnd_)
		return;

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
		, parent           // hWndParent
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

void CredentialsView::SetFocusToUsername()
{
	element(FindFirstElement("#username")).set_state(STATE_FOCUS);
}

void CredentialsView::SetMessage(const std::wstring & message)
{
	element root = element::root_element(hwnd_);
	element e = root.find_first("#message");
	if (!e)
		throw std::exception("#message not found.");
	e.set_text(message.c_str());
}

void CredentialsView::SetShard(const std::wstring & shard)
{
	element e(FindFirstElement("#shard"));
	if (!e)
		throw std::exception("#shard not found.");
	e.set_text(shard.c_str());
}

void CredentialsView::SetToken(const std::wstring & token)
{
	element e(FindFirstElement("#token"));
	if (!e)
		throw std::exception("#token not found.");
	e.set_text(token.c_str());
}

void CredentialsView::SetUsername(const std::wstring & username)
{
	element e(FindFirstElement("#username"));
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
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
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
			Rect rect(sipInfo.rcVisibleDesktop);
			::MoveWindow
				( hwnd_            // hwnd
				, rect.GetX()      // x
				, rect.GetY()      // y
				, rect.GetWidth()  // nWidth
				, rect.GetHeight() // nHeight
				, TRUE             // bRepaint
				);
		}
		else
		{
			HWND menuBar(::SHFindMenuBar(hwnd_));
			if (menuBar)
			{
				Rect desktopRect(sipInfo.rcVisibleDesktop);

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

void CredentialsView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
	{
		SignalCancel();
		msg.handled_ = true;
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
		&CredentialsView::OnKeyUp,
		&CredentialsView::OnSettingChange,
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

//---------------------------
// HTMLayout message handlers
//---------------------------

BOOL CredentialsView::OnKey(KEY_PARAMS * params)
{
	if (params->cmd & HANDLED)
		return FALSE;
	if (!(params->cmd & KEY_UP))
		return FALSE;
	if (params->key_code == 0x0D)
	{
		if (params->target == FindFirstElement("#username"))
		{
			element(FindFirstElement("#password")).set_state(STATE_FOCUS);
			return TRUE;
		}
		if (params->target == FindFirstElement("#password"))
		{
			SignalOk();
			return TRUE;
		}
	}
	return FALSE;
}
