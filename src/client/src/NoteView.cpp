﻿#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
#include "IAnimator.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

#include <fstream>

using namespace boost;
using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteView::NoteView
	( HINSTANCE   instance
	, bool        highRes
	, IAnimator & animator
	)
	: instance         (instance)
	, isDirty          (false)
	, isMaximized      (false)
	, gestureProcessor (animator)
	, parent           (NULL)
	, HTMLayoutWindow  (L"note-view.htm", highRes)
{

	gestureProcessor.ConnectDelayedMouseDown (bind(&NoteView::OnDelayedMouseDown, this));
	gestureProcessor.ConnectGestureStart     (bind(&NoteView::OnGestureStart,     this));
	gestureProcessor.ConnectGestureStep      (bind(&NoteView::OnGestureStep,      this));
}

void NoteView::Create(HWND parent)
{
	this->parent = parent;

	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	DWORD windowStyle   (WS_POPUP);
	DWORD windowExStyle (WS_EX_CAPTIONOKBTN);

	Rect rect;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, FALSE);

	hwnd_ = ::CreateWindowEx
		( windowExStyle    // dwExStyle
		, wndClass.c_str() // lpClassName
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
}

void NoteView::RegisterEventHandlers()
{
	ConnectBehavior("#edit",        BUTTON_CLICK, &NoteView::OnEdit);
	ConnectBehavior("#full-screen", BUTTON_CLICK, &NoteView::OnToggle);
	ConnectBehavior("#home",        BUTTON_CLICK, &NoteView::OnHome);
}

//-------------------------
// INoteView implementation
//-------------------------

void NoteView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void NoteView::ConnectEdit(slot_type OnEdit)
{
	SignalEdit.connect(OnEdit);
}

void NoteView::ConnectToggleMaximize(slot_type OnToggleMaximize)
{
	SignalToggleMaximize.connect(OnToggleMaximize);
}

static void CALLBACK _writer_a(LPCBYTE utf8, UINT utf8_length, LPVOID param)
{
	wstring * dst(reinterpret_cast<wstring*>(param));
	ConvertToUnicode(utf8, *dst);
}

void NoteView::GetBody(wstring & html)
{
	HTMLayoutGetElementHtmlCB
		( FindFirstElement("#note") // he
		, false                     // outer
		, _writer_a                 // cb
		, &html                     // cb_param
		);
}

void NoteView::GetNote(Note & note)
{
	note = this->note;
}

void NoteView::GetTitle(std::wstring & text)
{
	text = element(FindFirstElement("#title")).text();
}

void NoteView::Hide()
{
	if (!::IsWindowVisible(hwnd_))
		return;
	::EnableWindow(parent, TRUE);
	::ShowWindow(hwnd_, SW_HIDE);
	SignalClose();
}

bool NoteView::IsDirty()
{
	return isDirty;
}

bool NoteView::IsMaximized()
{
	return isMaximized;
}

void NoteView::MaximizeWindow()
{
	isMaximized = true;
	UpdateWindowState();
}

void NoteView::Render(Thumbnail & thumbnail)
{
	HTMLayoutUpdateElementEx
		( element::root_element(hwnd_)
		, RESET_STYLE_DEEP | MEASURE_DEEP | REDRAW_NOW
		);
	WindowRenderer::RenderThumbnail(hwnd_, thumbnail);
}

void NoteView::RestoreWindow()
{
	isMaximized = false;
	UpdateWindowState();
}

void NoteView::SetNote
	( const Note    & note
	, const wstring & titleText
	, const wstring & subtitleText
	, const wstring & bodyHtml
	)
{
	isDirty = false;

	this->note = note;

	element title    (FindFirstElement("#title"));
	element subtitle (FindFirstElement("#subtitle"));
	element body     (FindFirstElement("#body"));

	title.set_text    (titleText.c_str(),    titleText.size());
	subtitle.set_text (subtitleText.c_str(), subtitleText.size());

	vector<unsigned char> utf8Chars;
	const unsigned char * utf8 = Tools::ConvertToUtf8(bodyHtml, utf8Chars);

	DisconnectBehavior("#note input");
	body.set_html(utf8, utf8Chars.size());
	ConnectBehavior("#note input", BUTTON_STATE_CHANGED, &NoteView::OnInput);

	POINT scrollPos = { 0 };
	SetScrollPos(scrollPos);
	UpdateScrollbar();
}

void NoteView::SetWindowTitle(const std::wstring & text)
{
	::SetWindowText(hwnd_, text.c_str());
}

void NoteView::Show()
{
	if (::IsWindowVisible(hwnd_))
		return;

	::EnableWindow(parent, FALSE);
	::ShowWindow(hwnd_, SW_SHOW);
	::UpdateWindow(hwnd_);
	::BringWindowToTop(hwnd_);

	element root = element::root_element(hwnd_);
	root.update(true);
}

//------------------
// utility functions
//------------------

POINT NoteView::GetScrollPos()
{
	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	element body(FindFirstElement("#note"));
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	return scrollPos;
}

ATOM NoteView::RegisterClass(const wstring & wndClass)
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

void NoteView::SetScrollPos(POINT pos)
{
	element body    (FindFirstElement("#note"));
	element vScroll (FindFirstElement("#vscroll"));
	element vSlider (FindFirstElement("#vscroll > #slider"));
	element hScroll (FindFirstElement("#hscroll"));
	element hSlider (FindFirstElement("#hscroll > #slider"));

	POINT scrollPos;
	Rect  viewRect;
	SIZE  contentSize;
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	int contentHeight(contentSize.cy);

	Rect listRect(body.get_location(SCROLLABLE_AREA));
	if (listRect.GetWidth() == 0 || listRect.GetHeight() == 0)
		return;

	SIZE contentDistance =
		{ contentSize.cx - listRect.GetWidth()
		, contentSize.cy - listRect.GetHeight()
		};
	if (contentDistance.cx <= 0 || contentDistance.cy <= 0)
		return;

	pos.x = min(max(pos.x, 0), contentDistance.cx);
	pos.y = min(max(pos.y, 0), contentDistance.cy);
	body.set_scroll_pos(pos, false);

	Rect vScrollRect(vScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
	Rect vSliderRect(vSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

	__int64 vScrollDistance(vScrollRect.GetHeight() - vSliderRect.GetHeight());
	if (vScrollDistance <= 0L)
		return;

	POINT vScrollPos =
		{ 0
		, -static_cast<int>(pos.y * vScrollDistance / contentDistance.cy)
		};
	vScroll.set_scroll_pos(vScrollPos, false);

	Rect hScrollRect(hScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
	Rect hSliderRect(hSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

	__int64 hScrollDistance(hScrollRect.GetWidth() - hSliderRect.GetWidth());
	if (hScrollDistance <= 0L)
		return;

	POINT hScrollPos =
		{ -static_cast<int>(pos.x * hScrollDistance / contentDistance.cx)
		, 0
		};
	hScroll.set_scroll_pos(hScrollPos, false);
}

void NoteView::UpdateScrollbar()
{
	element body    (FindFirstElement("#note"));
	element vScroll (FindFirstElement("#vscroll"));
	element vSlider (FindFirstElement("#vscroll > #slider"));
	element hScroll (FindFirstElement("#hscroll"));
	element hSlider (FindFirstElement("#hscroll > #slider"));

	body.update(MEASURE_DEEP|REDRAW_NOW);

	Rect listRect(body.get_location(SCROLLABLE_AREA));
	if (listRect.GetWidth() == 0 || listRect.GetHeight() == 0)
		return;

	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	body.get_scroll_info(scrollPos, viewRect, contentSize);
	if (contentSize.cy > 0)
	{
		if (contentSize.cy <= listRect.GetHeight())
		{
			vSlider.set_style_attribute("display", L"none");
			return;
		}
		else
		{
			vSlider.set_style_attribute("display", L"block");
		}

		Rect scrollRect(vScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		if (scrollRect.GetHeight() == 0)
			return;

		int sliderHeight
			( static_cast<int>
				( static_cast<__int64>(scrollRect.GetHeight())
				* listRect.GetHeight() / contentSize.cy
				)
			);

		wchar_t text[16];
		_itow_s(sliderHeight, text, 16, 10);
		vSlider.set_style_attribute("height", text);
	}
	if (contentSize.cx > 0)
	{
		if (contentSize.cx <= listRect.GetWidth())
		{
			hSlider.set_style_attribute("display", L"none");
			return;
		}
		else
		{
			hSlider.set_style_attribute("display", L"block");
		}

		Rect scrollRect(hScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		if (scrollRect.GetWidth() == 0)
			return;

		int sliderWidth
			( static_cast<int>
				( static_cast<__int64>(scrollRect.GetWidth())
				* listRect.GetWidth() / contentSize.cx
				)
			);

		wchar_t text[16];
		_itow_s(sliderWidth, text, 16, 10);
		hSlider.set_style_attribute("height", text);
	}
}

void NoteView::UpdateWindowState()
{
	::SHFullScreen
		( hwnd_
		, isMaximized
		? SHFS_HIDESIPBUTTON | SHFS_HIDETASKBAR
		: SHFS_HIDESIPBUTTON | SHFS_SHOWTASKBAR
		);
	Rect rect;
	if (isMaximized)
	{
		rect.left   = 0;
		rect.top    = 0;
		rect.right  = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, FALSE);
	}
	::MoveWindow
		( hwnd_
		, rect.GetX()
		, rect.GetY()
		, rect.GetWidth()
		, rect.GetHeight()
		, TRUE
		);
	element img(FindFirstElement("#full-screen"));
	img.set_style_attribute
		( "background-image"
		, isMaximized
		? L"url(view-restore.png)"
		: L"url(view-fullscreen.png)"
		);
	img.update();
}

//-------------------------
// gesture message handlers
//-------------------------

void NoteView::OnDelayedMouseDown()
{
	__super::ProcessMessage(*gestureProcessor.GetMouseDownMessage());
}

void NoteView::OnGestureStart()
{
	startScrollPos = GetScrollPos();
}

void NoteView::OnGestureStep()
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

void NoteView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
		UpdateWindowState();
}

void NoteView::OnClose(Msg<WM_CLOSE> & msg)
{
	Hide();
	msg.handled_ = true;
}

void NoteView::OnCommand(Msg<WM_COMMAND> & msg)
{
	if (msg.CtrlId() == IDOK)
	{
		CloseWindow(hwnd_);
		msg.handled_ = true;
	}
}

void NoteView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	::SetCapture(hwnd_);

	gestureProcessor.OnMouseDown(msg);

	msg.handled_ = true;
}

void NoteView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	msg.handled_ = true;

	gestureProcessor.OnMouseMove(msg);
}

void NoteView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	gestureProcessor.OnMouseUp(msg);
}

void NoteView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteView::OnActivate,
		&NoteView::OnClose,
		&NoteView::OnCommand,
		&NoteView::OnMouseDown,
		&NoteView::OnMouseMove,
		&NoteView::OnMouseUp,
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

void NoteView::OnEdit(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalEdit();
}

void NoteView::OnToggle(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalToggleMaximize();
}

void NoteView::OnHome(BEHAVIOR_EVENT_PARAMS * params)
{
	CloseWindow(hwnd_);
}

void NoteView::OnInput(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	if (e.get_value().get(false))
		e.set_attribute("checked", L"true");
	else
		e.remove_attribute("checked");
	note.isDirty = true;
	isDirty      = true;
}
