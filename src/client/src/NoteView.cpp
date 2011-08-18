#include "stdafx.h"
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
	( HINSTANCE         instance
	, bool              highRes
	, IAnimator       & animator
	, IHtmlDataLoader & htmlDataLoader
	)
	: instance         (instance)
	, isDirty          (false)
	, isMaximized      (false)
	, gestureProcessor (animator)
	, parent           (NULL)
	, HTMLayoutWindow  (L"note-view.htm", highRes, htmlDataLoader)
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

	SHMENUBARINFO menuBarInfo = { sizeof(menuBarInfo) };
	menuBarInfo.hwndParent = hwnd_;
	menuBarInfo.dwFlags    = SHCMBF_HIDDEN | SHCMBF_HIDESIPBUTTON;
	menuBarInfo.nToolBarId = IDR_NOTE_VIEW_MENUBAR;
	menuBarInfo.hInstRes   = instance;
	::SHCreateMenuBar(&menuBarInfo);
	menuBar = menuBarInfo.hwndMB;

	UpdateWindowState();
}

void NoteView::RegisterEventHandlers()
{
	body    = FindFirstElement("#note");
	vScroll = FindFirstElement("#vscroll");
	vSlider = FindFirstElement("#vscroll > #slider");
	hScroll = FindFirstElement("#hscroll");
	hSlider = FindFirstElement("#hscroll > #slider");
}

//-------------------------
// INoteView implementation
//-------------------------

static void CALLBACK _writer_a(LPCBYTE utf8, UINT utf8_length, LPVOID param)
{
	wstring * dst(reinterpret_cast<wstring*>(param));
	ConvertToUnicode(utf8, *dst);
}

void NoteView::GetBody(wstring & html)
{
	HTMLayoutGetElementHtmlCB
		( FindFirstElement("#body") // he
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
	::ShowWindow(hwnd_,   SW_HIDE);
	::ShowWindow(menuBar, SW_HIDE);
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
	POINT scrollPos = { 0 };
	SetScrollPos(scrollPos);

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
	, const wstring & attachment
	, const bool      enableChrome
	)
{
	isDirty = false;

	this->note = note;

	SetChrome(enableChrome);

	element title    (FindFirstElement("#title"));
	element subtitle (FindFirstElement("#subtitle"));
	element source   (FindFirstElement("#source"));
	element body     (FindFirstElement("#body"));

	title.set_text    (titleText.c_str(),    titleText.size());
	subtitle.set_text (subtitleText.c_str(), subtitleText.size());

	if (note.SourceUrl.empty())
	{
		source.set_style_attribute("display", L"none");
	}
	else
	{
		source.set_style_attribute("display", L"block");
		source.set_text(note.SourceUrl.c_str(), note.SourceUrl.size());
		source.set_attribute("href", note.SourceUrl.c_str());
	}

	vector<unsigned char> utf8Chars;
	const unsigned char * utf8 = Tools::ConvertToUtf8(bodyHtml, utf8Chars);

	DisconnectBehavior("#body input");
	body.set_html(utf8, utf8Chars.size());
	ConnectBehavior("#body input", BUTTON_STATE_CHANGED, &NoteView::OnInput);

	vector<unsigned char> utf8AttachmentChars;
	const unsigned char * utf8Attachment = Tools::ConvertToUtf8(attachment, utf8AttachmentChars);

	element(FindFirstElement("#attachments")).set_html(utf8Attachment, utf8AttachmentChars.size());
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
	::BringWindowToTop(hwnd_);

	element(element::root_element(hwnd_)).update(MEASURE_DEEP|REDRAW_NOW);

	::ShowWindow(hwnd_,   SW_SHOW);
	::ShowWindow(menuBar, SW_SHOW);
	::UpdateWindow(hwnd_);

	POINT scrollPos = { 0 };
	SetScrollPos(scrollPos);
	UpdateScrollbar();
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

void NoteView::SetChrome(bool enable)
{
	element blocks[] =
		{ hScroll
		, vScroll
		, element(FindFirstElement("#header"))
		, element(FindFirstElement("#attachments"))
		};
	for (int i(0); i != GetArraySize(blocks); ++i)
		blocks[i].set_style_attribute("display", enable ? L"block" : L"none");
}

void NoteView::SetScrollPos(POINT pos)
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

	POINT hScrollPos = { 0 };
	POINT vScrollPos = { 0 };

	if (contentDistance.cx > 0)
	{
		Rect hScrollRect(hScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		Rect hSliderRect(hSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

		__int64 hScrollDistance(hScrollRect.GetWidth() - hSliderRect.GetWidth());
		if (hScrollDistance > 0L)
			hScrollPos.x =-static_cast<int>(pos.x * hScrollDistance / contentDistance.cx);
	}

	if (contentDistance.cy > 0)
	{
		Rect vScrollRect(vScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		Rect vSliderRect(vSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));

		__int64 vScrollDistance(vScrollRect.GetHeight() - vSliderRect.GetHeight());
		if (vScrollDistance > 0L)
			vScrollPos.y = -static_cast<int>(pos.y * vScrollDistance / contentDistance.cy);
	}

	hScroll.set_scroll_pos(hScrollPos, false);
	vScroll.set_scroll_pos(vScrollPos, false);
}

void NoteView::UpdateScrollbar()
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

	if (contentSize.cx > bodyRect.GetWidth())
	{
			
		hSlider.set_style_attribute("display", L"block");

		Rect scrollRect(hScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
		if (scrollRect.GetWidth() > 0)
		{
			int sliderWidth
				( static_cast<int>
					( static_cast<__int64>(scrollRect.GetWidth())
					* bodyRect.GetWidth() / contentSize.cx
					)
				);
			wchar_t text[16];
			_itow_s(sliderWidth, text, 16, 10);
			hSlider.set_style_attribute("width", text);
		}
	}
	else
	{
		hSlider.set_style_attribute("display", L"none");
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
	if (menuBar)
	{
		Rect menuBarRect;
		::GetWindowRect(menuBar, &menuBarRect);
		rect.bottom -= menuBarRect.GetHeight();
	}
	::MoveWindow
		( hwnd_
		, rect.GetX()
		, rect.GetY()
		, rect.GetWidth()
		, rect.GetHeight()
		, TRUE
		);
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
	{
		UpdateWindowState();
		msg.handled_ = true;
	}
}

void NoteView::OnClose(Msg<WM_CLOSE> & msg)
{
	Hide();
	msg.handled_ = true;
}

void NoteView::OnCommand(Msg<WM_COMMAND> & msg)
{
	switch (msg.CtrlId())
	{
	case IDOK:           CloseWindow(hwnd_);     break;
	case IDM_OK:         CloseWindow(hwnd_);     break;
	case IDM_CANCEL:     CloseWindow(hwnd_);     break;
	case ID_EDIT_NOTE:   SignalEdit();           break;
	case ID_FULL_SCREEN: SignalToggleMaximize(); break;
	}
}

void NoteView::OnKeyUp(Msg<WM_KEYUP> & msg)
{
	if (msg.VKey() == 0x1b)
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

void NoteView::OnSettingChange(Msg<WM_SETTINGCHANGE> & msg)
{
	if (msg.Flag() == SPI_SETSIPINFO)
	{
		UpdateWindowState();
		msg.handled_ = true;
	}
}

void NoteView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteView::OnActivate,
		&NoteView::OnClose,
		&NoteView::OnCommand,
		&NoteView::OnKeyUp,
		&NoteView::OnMouseDown,
		&NoteView::OnMouseMove,
		&NoteView::OnMouseUp,
		&NoteView::OnSettingChange,
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
