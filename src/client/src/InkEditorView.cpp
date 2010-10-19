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
	: instance (instance)
	, penColor (PenBlack)
	, penWidth (Pen1px)
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

void InkEditorView::ConnectPenChanged(slot_type OnPenChanged)
{
	SignalPenChanged.connect(OnPenChanged);
}

void InkEditorView::GetImage(Blob & blob)
{
	// calculate the rectangle to return
	Rect cropRect(drawingBounds);
	::InflateRect(&cropRect, 4, 4);
	
	Rect clientRect;
	::GetClientRect(hwnd_, &clientRect);

	Rect rect;
	::IntersectRect(&rect, &clientRect, &cropRect);

	// create a new bitmap
 	BITMAPINFO_BF info = { 0 };
	info.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth        = rect.GetWidth();
	info.bmiHeader.biHeight       = rect.GetHeight();
	info.bmiHeader.biPlanes       = 1;
	info.bmiHeader.biBitCount     = 16;
	info.bmiHeader.biCompression  = BI_BITFIELDS;
	info.bmiHeader.biSizeImage    = ((rect.GetWidth() * 2 + 3) & ~3) * rect.GetHeight();
	info.bmiHeader.biClrUsed      = 1;
	info.bmiHeader.biClrImportant = 0;
	info.bmiColorsR               = 0xf800;
	info.bmiColorsG               = 0x07e0;
	info.bmiColorsB               = 0x001f;

	HDC windowDc(::GetDC(hwnd_));
	HDC dstDc(::CreateCompatibleDC(windowDc));
	::ReleaseDC(hwnd_, windowDc);

	HBITMAP newBmp = ::CreateDIBSection
		( dstDc                // hdc
		, info.GetBitmapInfo() // pbmi
		, DIB_RGB_COLORS       // iUsage
		, NULL                 // ppvBits
		, NULL                 // hSection
		, 0                    // dwOffset
		);
	::SelectObject(dstDc, newBmp);

	gfx->BitBlt
		( dstDc
		, 0
		, 0
		, rect.GetX()
		, rect.GetY()
		, rect.GetWidth()
		, rect.GetHeight()
		);

	::DeleteDC(dstDc);

	WindowRenderer::Render(newBmp, blob);
}

InkPenColor InkEditorView::GetPenColor()
{
	return penColor;
}

InkPenWidth InkEditorView::GetPenWidth()
{
	return penWidth;
}

void InkEditorView::Hide()
{
	if (hwnd_)
	{
		::CloseWindow(hwnd_);
		hwnd_ = NULL;
	}
}

void InkEditorView::SetPen(InkPenWidth width, InkPenColor color)
{
	penWidth = width;
	penColor = color;
	gfx->SetLineWidth(GetPenWidth(width));
	gfx->SetLineColor(GetPenColor(color));
	SetPenWidthMenuState(width);
	SetPenColorMenuState(color);
}

void InkEditorView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_INK_EDIT);

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
	menuBarInfo.nToolBarId = IDR_INK_EDITOR_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);
	menuBar = menuBarInfo.hwndMB;

	isDrawing      = false;
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

	case ID_PENWIDTH_1PX: penWidth = Pen1px; SignalPenChanged(); break;
	case ID_PENWIDTH_2PX: penWidth = Pen2px; SignalPenChanged(); break;
	case ID_PENWIDTH_4PX: penWidth = Pen4px; SignalPenChanged(); break;
	case ID_PENWIDTH_8PX: penWidth = Pen8px; SignalPenChanged(); break;

	case ID_COLOR_BLACK:  penColor = PenBlack;  SignalPenChanged(); break;
	case ID_COLOR_WHITE:  penColor = PenWhite;  SignalPenChanged(); break;
	case ID_COLOR_YELLOW: penColor = PenYellow; SignalPenChanged(); break;
	case ID_COLOR_RED:    penColor = PenRed;    SignalPenChanged(); break;
	case ID_COLOR_BLUE:   penColor = PenBlue;   SignalPenChanged(); break;
	case ID_COLOR_GREEN:  penColor = PenGreen;  SignalPenChanged(); break;
	}
}

void InkEditorView::OnEraseBackground(Msg<WM_ERASEBKGND> & msg)
{
	msg.handled_ = true;
}

void InkEditorView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	isDrawing = true;

	::SetCapture(hwnd_);

	lineStart = msg.Position();
}

void InkEditorView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	if (!isDrawing)
		return;

	Rect rect;

	rect.left = lineStart.x;
	rect.top  = lineStart.y;

	lineStart = msg.Position();
	AddToDrawingBounds(lineStart);

	rect.right  = lineStart.x;
	rect.bottom = lineStart.y;

	gfx->DrawLine(rect.left, rect.top, rect.right, rect.bottom);

	rect.Normalize();
	int radius(GetPenWidth(penWidth) / 2 + 1);
	::InflateRect(&rect, radius, radius);

	::InvalidateRect(hwnd_, &rect, FALSE);
}

void InkEditorView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	if (!isDrawing)
		return;

	::ReleaseCapture();

	isDrawing = false;
}

void InkEditorView::OnPaint(Msg<WM_PAINT> & msg)
{
	msg.BeginPaint(hwnd_);
	Rect rect(msg.PS().rcPaint);
	gfx->BitBlt
		( msg.PS().hdc
		, rect.GetX()
		, rect.GetY()
		, rect.GetX()
		, rect.GetY()
		, rect.GetWidth()
		, rect.GetHeight()
		);
	msg.EndPaint();
}

void InkEditorView::OnSize(Msg<WM_SIZE> & msg)
{
	gfx.reset(new GraphinGfx(msg.Size().cx, msg.Size().cy));
	gfx->SetFillColor(0xFFFFFFFF);
	gfx->DrawRectangle(0, 0, msg.Size().cx, msg.Size().cy);
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

COLORREF InkEditorView::GetPenColor(InkPenColor color)
{
	switch (color)
	{
	case PenBlack:  return 0xFF000000;
	case PenWhite:  return 0xFFFFFFFF;
	case PenYellow: return 0xFF00FFFF;
	case PenRed:    return 0xFF0000FF;
	case PenGreen:  return 0xFF00FF00;
	case PenBlue:   return 0xFFFF0000;
	}
	return 0xFF000000;
}

int InkEditorView::GetPenWidth(InkPenWidth width)
{
	switch (width)
	{
	case Pen1px: return 1;
	case Pen2px: return 2;
	case Pen4px: return 4;
	case Pen8px: return 8;
	}
	return 1;
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

void InkEditorView::SetPenColorMenuState(InkPenColor color)
{
	int target;
	switch (color)
	{
	case PenBlack:  target = ID_COLOR_BLACK;  break;
	case PenWhite:  target = ID_COLOR_WHITE;  break;
	case PenYellow: target = ID_COLOR_YELLOW; break;
	case PenRed:    target = ID_COLOR_RED;    break;
	case PenBlue:   target = ID_COLOR_BLUE;   break;
	case PenGreen:  target = ID_COLOR_GREEN;  break;
	default: return;
	}

	int options[] =
		{ ID_COLOR_BLACK
		, ID_COLOR_WHITE
		, ID_COLOR_YELLOW
		, ID_COLOR_RED
		, ID_COLOR_BLUE
		, ID_COLOR_GREEN
		};

	HMENU menu(::GetWindowMenu(menuBar, IDM_MENU));
	for (int i(0); i != GetArraySize(options); ++i)
	{
		UINT state((options[i] == target) ? MF_CHECKED : MF_UNCHECKED);
		::CheckMenuItem(menu, options[i], state);
	}
}

void InkEditorView::SetPenWidthMenuState(InkPenWidth width)
{
	int target;
	switch (width)
	{
	case Pen1px: target = ID_PENWIDTH_1PX; break;
	case Pen2px: target = ID_PENWIDTH_2PX; break;
	case Pen4px: target = ID_PENWIDTH_4PX; break;
	case Pen8px: target = ID_PENWIDTH_8PX; break;
	default: return;
	}

	int options[] =
		{ ID_PENWIDTH_1PX
		, ID_PENWIDTH_2PX
		, ID_PENWIDTH_4PX
		, ID_PENWIDTH_8PX
		};

	HMENU menu(::GetWindowMenu(menuBar, IDM_MENU));
	for (int i(0); i != GetArraySize(options); ++i)
	{
		UINT state((options[i] == target) ? MF_CHECKED : MF_UNCHECKED);
		::CheckMenuItem(menu, options[i], state);
	}
}
