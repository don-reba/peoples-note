#include "stdafx.h"
#include "CredentialsView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

CredentialsView::CredentialsView(HINSTANCE instance)
	: instance        (instance)
	, parent          (NULL)
	, HTMLayoutWindow (L"signin-view.htm")
{
}

void CredentialsView::Create(HWND parent)
{
	wstring wndClass = LoadStringResource(IDC_CREDENTIALS_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	this->parent = parent;
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
}

void CredentialsView::ConnectCreated(slot_type OnCreated)
{
}

void CredentialsView::ConnectOk(slot_type OnOk)
{
}

wstring CredentialsView::GetPassword() const
{
	// TODO: implement
	return L"";
}

wstring CredentialsView::GetUsername() const
{
	// TODO: implement
	return L"";
}

void CredentialsView::Open()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_CREDENTIALS_VIEW);

	DWORD windowStyle(WS_VISIBLE/*WS_NONAVDONEBUTTON*/);

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

	//CopyParentSize();
}

void CredentialsView::SetMessage(const std::wstring & message)
{
}

void CredentialsView::SetPassword(const std::wstring & password)
{
}

void CredentialsView::SetUsername(const std::wstring & username)
{
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

//------------------------
// window message handlers
//------------------------

void CredentialsView::ProcessMessage(WndMsg &msg)
{
	__super::ProcessMessage(msg);
}
