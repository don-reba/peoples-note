#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Tools.h"

#include <fstream>

using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteView::NoteView(HINSTANCE instance, bool highRes)
	: instance        (instance)
	, isDirty         (false)
	, isFullScreen    (false)
	, parent          (NULL)
	, HTMLayoutWindow (L"note-view.htm", highRes)
{
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
	ConnectBehavior("#full-screen", BUTTON_CLICK, &NoteView::OnFullScreen);
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

static void CALLBACK _writer_a(LPCBYTE utf8, UINT utf8_length, LPVOID param)
{
	wstring * dst(reinterpret_cast<wstring*>(param));
	*dst = ConvertToUnicode(utf8);
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
	::ShowWindow(hwnd_, SW_HIDE);
	SignalClose();
}

bool NoteView::IsDirty()
{
	return isDirty;
}

void NoteView::Render(Thumbnail & thumbnail)
{
	HTMLayoutUpdateElementEx
		( element::root_element(hwnd_)
		, RESET_STYLE_DEEP | MEASURE_DEEP | REDRAW_NOW
		);
	WindowRenderer::RenderThumbnail(hwnd_, thumbnail);
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

	DisconnectBehavior("#body input");
	body.set_html(utf8, utf8Chars.size());
	ConnectBehavior("#body input", BUTTON_STATE_CHANGED, &NoteView::OnInput);
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

void NoteView::UpdateFullScreen()
{
	::SHFullScreen
		( hwnd_
		, isFullScreen
		? SHFS_HIDESIPBUTTON | SHFS_HIDETASKBAR
		: SHFS_HIDESIPBUTTON | SHFS_SHOWTASKBAR
		);
	Rect rect;
	if (isFullScreen)
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
		, isFullScreen
		? L"url(view-restore.png)"
		: L"url(view-fullscreen.png)"
		);
	img.update();
}

//------------------------
// window message handlers
//------------------------

void NoteView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
		UpdateFullScreen();
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

void NoteView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteView::OnActivate,
		&NoteView::OnClose,
		&NoteView::OnCommand,
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

void NoteView::OnFullScreen(BEHAVIOR_EVENT_PARAMS * params)
{
	isFullScreen = !isFullScreen;
	UpdateFullScreen();
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
