#include "stdafx.h"
#include "InkEditorView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace std;
using namespace Tools;

//----------
// interface
//----------

InkEditorView::InkEditorView(HINSTANCE instance)
	: bmp      (NULL)
	, bmpDc    (NULL)
	, instance (instance)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
}

void InkEditorView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_INK_EDIT)))
		throw std::exception("Class could not be registered.");
}

//------------------------------
// IInkEditorView implementation
//------------------------------

void InkEditorView::ConnectAccept(slot_type OnAccept)
{
	SignalAccept.connect(OnAccept);
}

void InkEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void InkEditorView::Hide()
{
	::DeleteObject(bmp);
	bmp = NULL;

	::DeleteDC(bmpDc);
	bmpDc = NULL;

	CloseWindow(hwnd_);
	hwnd_ = NULL;
}

void InkEditorView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_EDIT);

	DWORD windowStyle(WS_POPUP);

	Rect rect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, FALSE);

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, wndTitle.c_str() // lpWindowName
		, windowStyle      // dwStyle
		, rect.GetX()      // x
		, rect.GetY()      // y
		, rect.GetWidth()  // nWidth
		, rect.GetHeight() // nHeight
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
	menuBar = menuBarInfo.hwndMB;

	ShowWindow(hwnd_, SW_SHOW);
}

//------------------------
// window message handlers
//------------------------
void InkEditorView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, SHFS_SHOWSIPBUTTON);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
}

void InkEditorView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDM_OK:     SignalAccept(); break;
	case IDM_CANCEL: SignalCancel(); break;
	}
}

void InkEditorView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	::SetCapture(hwnd_);

	lineStart = msg.Position();
	::MoveToEx(bmpDc, lineStart.x, lineStart.y, NULL);
}

void InkEditorView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	RECT rect;

	rect.left = lineStart.x;
	rect.top  = lineStart.y;

	lineStart = msg.Position();

	rect.right  = lineStart.x;
	rect.bottom = lineStart.y;

	if (rect.right < rect.left)
		swap(rect.left, rect.right);
	if (rect.bottom < rect.top)
		swap(rect.top, rect.bottom);

	const LONG padding(1);
	rect.left   -= padding;
	rect.top    -= padding;
	rect.right  += padding;
	rect.bottom += padding;

	::LineTo(bmpDc, lineStart.x, lineStart.y);
	::InvalidateRect(hwnd_, &rect, FALSE);
}

void InkEditorView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();
}

void InkEditorView::OnPaint(Msg<WM_PAINT> & msg)
{
	Rect rect;
	::GetClientRect(hwnd_, &rect);

	msg.BeginPaint(hwnd_);
	::BitBlt
		( msg.PS().hdc
		, rect.GetX()
		, rect.GetY()
		, rect.GetWidth()
		, rect.GetHeight()
		, bmpDc
		, rect.GetX()
		, rect.GetY()
		, SRCCOPY
		);
	msg.EndPaint();
}

void InkEditorView::OnSize(Msg<WM_SIZE> & msg)
{
	// create a new bitmap
	if (bmpDc)
		::DeleteDC(bmpDc);
	bmpDc = ::CreateCompatibleDC(::GetDC(hwnd_));
	::SelectObject(bmpDc, ::GetStockObject(BLACK_PEN));

	if (bmp)
		::DeleteObject(bmp);
	bmp = ::CreateCompatibleBitmap(bmpDc, msg.Size().cx, msg.Size().cy);
	::SelectObject(bmpDc, bmp);

	RECT rect = { 0, 0, msg.Size().cx, msg.Size().cy };
	::FillRect(bmpDc, &rect, static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)));
}

void InkEditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&InkEditorView::OnActivate,
		&InkEditorView::OnCommand,
		&InkEditorView::OnMouseDown,
		&InkEditorView::OnMouseMove,
		&InkEditorView::OnMouseUp,
		&InkEditorView::OnPaint,
		&InkEditorView::OnSize,
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

ATOM InkEditorView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &InkEditorView::WndProc<InkEditorView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}
