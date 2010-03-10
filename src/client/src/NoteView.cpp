#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteView::NoteView(HINSTANCE instance)
: instance        (instance)
, HTMLayoutWindow (L"note-view.htm")
{
}

void NoteView::Create(HWND parent)
{
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, L""              // lpWindowName
		, WS_VISIBLE       // dwStyle
		, CW_USEDEFAULT    // x
		, CW_USEDEFAULT    // y
		, CW_USEDEFAULT    // nWidth
		, CW_USEDEFAULT    // nHeight
		, NULL           // hWndParent
		, NULL             // hMenu
		, instance         // hInstance
		, this             // lpParam
		);
	if (!hwnd_)
		throw std::exception("Window creation failed.");

	this->parent = parent;

	CopyParentSize();

	Hide();
}

//-------------------------
// INoteView implementation
//-------------------------

void NoteView::Hide()
{
	::ShowWindow(hwnd_, SW_HIDE);
}

void NoteView::SetBody(wstring html)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element body = root.find_first("body");
	if (!body)
		throw std::exception("Html body not found.");

	
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = Tools::ConvertToUtf8(html, htmlUtf8Chars);

	body.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteView::SetTitle(wstring text)
{
	::SetWindowText(hwnd_, text.c_str());
}

void NoteView::Show()
{
	RECT rect;
	::GetWindowRect(hwnd_, &rect);
	::ShowWindow(hwnd_, SW_SHOW);
	::UpdateWindow(hwnd_);
	::BringWindowToTop(hwnd_);
}

//------------------
// utility functions
//------------------

void NoteView::CopyParentSize()
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

void NoteView::CreateMenuBar()
{
	menuBar = NULL;

	SHMENUBARINFO mbi = { sizeof(SHMENUBARINFO) };
	mbi.hwndParent = hwnd_;
	mbi.nToolBarId = IDR_MENU;
	mbi.hInstRes   = instance;
	if (!::SHCreateMenuBar(&mbi))
		throw exception("Menu bar could not be created.");
	menuBar = mbi.hwndMB;
}

ATOM NoteView::RegisterClass(wstring wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &NoteView::WndProc<NoteView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

//------------------------
// window message handlers
//------------------------

void NoteView::OnCommand(Msg<WM_COMMAND> &msg)
{
	switch (msg.GetCtrlId())
	{
	case IDM_CLOSE:
		Hide();
		break;
	}
}

void NoteView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteView::OnCommand,
	};
	if (!Handler::Call(mmp, this, msg))
		__super::ProcessMessage(msg);
}
