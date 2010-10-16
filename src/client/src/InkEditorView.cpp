#include "stdafx.h"
#include "InkEditorView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"
#include "WindowRenderer.h"

#include <sstream>

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

void InkEditorView::GetImage(Blob & blob)
{
	RECT cropRect(drawingBounds);
	::InflateRect(&cropRect, 4, 4);
	WindowRenderer::Render(bmp, cropRect, blob);
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

	isDrawingEmpty = true;

	ResizeWindow();

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

void InkEditorView::OnEraseBackground(Msg<WM_ERASEBKGND> & msg)
{
	msg.handled_ = true;
}

void InkEditorView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	::SetCapture(hwnd_);

	drawTime   = 0;
	drawCount  = 0;
	paintTime  = 0;
	paintCount = 0;

	lineStart = msg.Position();
	AddToDrawingBounds(lineStart);
	::SetPixel(bmpDc, lineStart.x, lineStart.y, 0xFF000000);
	::MoveToEx(bmpDc, lineStart.x, lineStart.y, NULL);
}

void InkEditorView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	DWORD startTime(::GetTickCount());

	Rect rect;

	rect.left = lineStart.x;
	rect.top  = lineStart.y;

	lineStart = msg.Position();
	AddToDrawingBounds(lineStart);

	rect.right  = lineStart.x;
	rect.bottom = lineStart.y;

	rect.Normalize();
	::InflateRect(&rect, 1, 1);

	::LineTo(bmpDc, lineStart.x, lineStart.y);
	::InvalidateRect(hwnd_, &rect, FALSE);

	drawTime += ::GetTickCount() - startTime;
	++drawCount;
}

void InkEditorView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	wostringstream stream;
	stream << L"draw: "  << (1000.0 * drawCount  / drawTime)  << L" fps\n";
	stream << L"paint: " << (1000.0 * paintCount / paintTime) << L" fps";
	DisplayMessage(stream.str().c_str());
}

void InkEditorView::OnPaint(Msg<WM_PAINT> & msg)
{
	DWORD startTime(::GetTickCount());

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

	paintTime += ::GetTickCount() - startTime;
	++paintCount;
}

void InkEditorView::OnSize(Msg<WM_SIZE> & msg)
{
	// create a new bitmap
	HDC windowDc(::GetDC(hwnd_));

	if (bmpDc)
		::DeleteDC(bmpDc);
	bmpDc = ::CreateCompatibleDC(windowDc);
	::SelectObject(bmpDc, ::GetStockObject(BLACK_PEN));

	if (bmp)
		::DeleteObject(bmp);
 	BITMAPINFO_BF info = { 0 };
	info.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth        = msg.Size().cx;
	info.bmiHeader.biHeight       = msg.Size().cy;
	info.bmiHeader.biPlanes       = 1;
	info.bmiHeader.biBitCount     = 16;
	info.bmiHeader.biCompression  = BI_BITFIELDS;
	info.bmiHeader.biSizeImage    = ((msg.Size().cx * 2 + 3) & ~3) * msg.Size().cy;
	info.bmiHeader.biClrUsed      = 1;
	info.bmiHeader.biClrImportant = 0;
	info.bmiColorsR               = 0xf800;
	info.bmiColorsG               = 0x07e0;
	info.bmiColorsB               = 0x001f;

	HDC dc(::CreateCompatibleDC(windowDc));
	bmp = ::CreateDIBSection
		( dc                   // hdc
		, info.GetBitmapInfo() // pbmi
		, DIB_RGB_COLORS       // iUsage
		, NULL                 // ppvBits
		, NULL                 // hSection
		, 0                    // dwOffset
		);
	::SelectObject(bmpDc, bmp);

	::ReleaseDC(hwnd_, windowDc);

	RECT rect = { 0, 0, msg.Size().cx, msg.Size().cy };
	::FillRect(bmpDc, &rect, static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)));
}

void InkEditorView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&InkEditorView::OnActivate,
		&InkEditorView::OnCommand,
		&InkEditorView::OnEraseBackground,
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


void InkEditorView::AddToDrawingBounds(const POINT & point)
{
	if (isDrawingEmpty)
	{
		isDrawingEmpty = false;

		drawingBounds.left = drawingBounds.right  = point.x;
		drawingBounds.top  = drawingBounds.bottom = point.y;
	}
	else
	{
		if (point.x < drawingBounds.left)
			drawingBounds.left = point.x;
		if (point.x > drawingBounds.right)
			drawingBounds.right = point.x;
		if (point.y < drawingBounds.top)
			drawingBounds.top = point.y;
		if (point.y > drawingBounds.bottom)
			drawingBounds.bottom = point.y;
	}
}

void InkEditorView::DisplayMessage(const wchar_t * text)
{
	RECT rect;
	::GetClientRect(hwnd_, &rect);
	::DrawText(bmpDc, text, -1, &rect, DT_NOCLIP);
	::InvalidateRect(hwnd_, &rect, FALSE);
}

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

void InkEditorView::ResizeWindow()
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
