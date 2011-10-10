#include "stdafx.h"
#include "NoteTagListView.h"

#include "crackers.h"
#include "IAnimator.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

#include <algorithm>
#include <fstream>
#include <iterator>

using namespace boost;
using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteTagListView::NoteTagListView
	( HINSTANCE         instance
	, bool              highRes
	, IAnimator       & animator
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance         (instance)
	, isDirty          (false)
	, gestureProcessor (animator)
	, parent           (NULL)
	, HTMLayoutWindow  (L"tags-view.htm", highRes, htmlDataLoader)
{
	gestureProcessor.ConnectDelayedMouseDown (bind(&NoteTagListView::OnDelayedMouseDown, this));
	gestureProcessor.ConnectGestureStart     (bind(&NoteTagListView::OnGestureStart,     this));
	gestureProcessor.ConnectGestureStep      (bind(&NoteTagListView::OnGestureStep,      this));
}

void NoteTagListView::Create(HWND parent)
{
	this->parent = parent;
	if (!RegisterClass(LoadStringResource(IDC_TAGS_VIEW)))
		throw std::exception("Class could not be registered.");
}

void NoteTagListView::RegisterEventHandlers()
{
	body    = FindFirstElement("#tag-panel");
	vScroll = FindFirstElement("#vscroll");
	vSlider = FindFirstElement("#vscroll > #slider");
}

//--------------------------------
// INoteTagListView implementation
//--------------------------------

void NoteTagListView::Hide()
{
	if (hwnd_)
	{
		CloseWindow(hwnd_);
		hwnd_ = NULL;
	}
}

wstring NoteTagListView::GetActiveTag()
{
	return L"";
}

void NoteTagListView::SetTags
	( const TagList & cleared
	, const TagList & selected
	)
{
}

void NoteTagListView::Show()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_TAGS_VIEW);

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

	::SHDoneButton(hwnd_, SHDB_SHOWCANCEL);

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.nToolBarId = IDR_OK_CANCEL_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);

	UpdateWindowState();

	ShowWindow(hwnd_, SW_SHOW);
}

//------------------
// utility functions
//------------------

POINT NoteTagListView::GetScrollPos()
{
	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	element body(FindFirstElement("#note"));
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	return scrollPos;
}

ATOM NoteTagListView::RegisterClass(const wstring & wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &NoteTagListView::WndProc<NoteTagListView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void NoteTagListView::SetScrollPos(POINT pos)
{
	POINT scrollPos;
	Rect  viewRect;
	SIZE  contentSize;
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	int contentHeight(contentSize.cy);

	Rect bodyRect(body.get_location(SCROLLABLE_AREA));
	if (bodyRect.GetWidth() == 0 || bodyRect.GetHeight() == 0)
		return;

	SIZE contentDistance =
		{ contentSize.cx - bodyRect.GetWidth()
		, contentSize.cy - bodyRect.GetHeight()
		};

	pos.x = max(min(pos.x, contentDistance.cx), 0);
	pos.y = max(min(pos.y, contentDistance.cy), 0);
	body.set_scroll_pos(pos, false);

	POINT vScrollPos = { 0 };

	if (contentDistance.cy > 0)
	{
		Rect vScrollRect(vScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		Rect vSliderRect(vSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

		__int64 vScrollDistance(vScrollRect.GetHeight() - vSliderRect.GetHeight());
		if (vScrollDistance > 0L)
			vScrollPos.y = -static_cast<int>(pos.y * vScrollDistance / contentDistance.cy);
	}

	vScroll.set_scroll_pos(vScrollPos, false);
}

void NoteTagListView::UpdateScrollbar()
{
	Rect bodyRect(body.get_location(SCROLLABLE_AREA));
	if (bodyRect.GetWidth() == 0 || bodyRect.GetHeight() == 0)
		return;

	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	if (contentSize.cy > bodyRect.GetHeight())
	{
		vSlider.set_style_attribute("display", L"block");

		Rect scrollRect(vScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		if (scrollRect.GetHeight() > 0)
		{
			int sliderHeight
				( static_cast<int>
					( static_cast<__int64>(scrollRect.GetHeight())
					* bodyRect.GetHeight() / contentSize.cy
					)
				);
			wchar_t text[16];
			_itow_s(sliderHeight, text, 16, 10);
			vSlider.set_style_attribute("height", text);
		}
	}
	else
	{
		vSlider.set_style_attribute("display", L"none");
	}
}

void NoteTagListView::UpdateWindowState()
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

//-------------------------
// gesture message handlers
//-------------------------

void NoteTagListView::OnDelayedMouseDown()
{
	__super::ProcessMessage(*gestureProcessor.GetMouseDownMessage());
}

void NoteTagListView::OnGestureStart()
{
	startScrollPos = GetScrollPos();
}

void NoteTagListView::OnGestureStep()
{
	SIZE offset(gestureProcessor.GetScrollDistance());
	POINT distance =
		{ startScrollPos.x + offset.cx
		, startScrollPos.y + offset.cy
		};
	SetScrollPos(distance);
}

//------------------------
// window message handlers
//------------------------

void NoteTagListView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		UpdateWindowState();
		msg.handled_ = true;
	}
}

void NoteTagListView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDOK:     case IDM_OK:     SignalOk();     break;
	case IDCANCEL: case IDM_CANCEL: SignalCancel(); break;
	}
}

void NoteTagListView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b) // Esc
	{
		SignalCancel();
		msg.handled_ = true;
	}
}

void NoteTagListView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	::SetCapture(hwnd_);

	gestureProcessor.OnMouseDown(msg);

	msg.handled_ = true;
}

void NoteTagListView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	msg.handled_ = true;

	gestureProcessor.OnMouseMove(msg);
}

void NoteTagListView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	gestureProcessor.OnMouseUp(msg);
}

void NoteTagListView::OnSettingChange(Msg<WM_SETTINGCHANGE> & msg)
{
	if (msg.Flag() == SPI_SETSIPINFO)
	{
		UpdateWindowState();
		msg.handled_ = true;
	}
}

void NoteTagListView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteTagListView::OnActivate,
		&NoteTagListView::OnCommand,
		&NoteTagListView::OnKeyUp,
		&NoteTagListView::OnMouseDown,
		&NoteTagListView::OnMouseMove,
		&NoteTagListView::OnMouseUp,
		&NoteTagListView::OnSettingChange,
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
