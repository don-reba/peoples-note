﻿#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
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

NoteView::NoteView(HINSTANCE instance)
	: instance        (instance)
	, isDirty         (false)
	, isFullScreen    (false)
	, parent          (NULL)
	, HTMLayoutWindow (L"note-view.htm")
{
}

void NoteView::Create(HWND parent)
{
	this->parent = parent;

	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	DWORD windowStyle(WS_NONAVDONEBUTTON);

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
}

void NoteView::RegisterEventHandlers()
{
	ConnectBehavior("#home",        BUTTON_CLICK, &NoteView::OnHome);
	ConnectBehavior("#full-screen", BUTTON_CLICK, &NoteView::OnFullScreen);
}

//-------------------------
// INoteView implementation
//-------------------------

void NoteView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void NoteView::ConnectLoadingData(DataSlot OnLoadingData)
{
	SignalLoadingData.connect(OnLoadingData);
}

static void CALLBACK _writer_a(LPCBYTE utf8, UINT utf8_length, LPVOID param)
{
	wstring * dst(reinterpret_cast<wstring*>(param));
	*dst = ConvertToUnicode(utf8);
}

void NoteView::GetBody(wstring & html)
{
	element root (element::root_element(hwnd_));
	element body (root.find_first("#body"));
	if (!body)
		throw std::exception("#body not found.");

	HTMLayoutGetElementHtmlCB(body, false, _writer_a, &html);

	wofstream note(L"My Documents\\People's Note\\note.txt");
	note << html;
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
	windowRenderer.Render(hwnd_, thumbnail);
}

void NoteView::SetBody(const wstring & html)
{
	element root (element::root_element(hwnd_));
	element body (root.find_first("#body"));
	if (!body)
		throw std::exception("#body not found.");

	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = Tools::ConvertToUtf8(html, htmlUtf8Chars);

	DisconnectBehavior("#body input");

	body.set_html(htmlUtf8, htmlUtf8Chars.size());

	ConnectBehavior("#body input", BUTTON_STATE_CHANGED, &NoteView::OnInput);
}

void NoteView::SetSubtitle(const wstring & text)
{
	element root = element::root_element(hwnd_);
	element body = root.find_first("#subtitle");
	if (!body)
		throw std::exception("#subtitle not found.");
	body.set_text(text.c_str(), text.size());
}

void NoteView::SetTitle(const wstring & text)
{
	element root = element::root_element(hwnd_);
	element body = root.find_first("#title");
	if (!body)
		throw std::exception("#title not found.");
	body.set_text(text.c_str(), text.size());
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

void NoteView::ToggleFullScreen()
{
	isFullScreen = !isFullScreen;
	::SHFullScreen
		( hwnd_
		, isFullScreen
		? SHFS_HIDESIPBUTTON | SHFS_HIDETASKBAR
		: SHFS_HIDESIPBUTTON | SHFS_SHOWTASKBAR
		);
	RECT rect = { 0 };
	if (isFullScreen)
	{
		rect.right  = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, FALSE);
	}
	::MoveWindow
		( hwnd_
		, rect.left
		, rect.top
		, rect.right  - rect.left
		, rect.bottom - rect.top
		, TRUE
		);
	element root = element::root_element(hwnd_);
	element img  = root.find_first("#full-screen img");
	if (img)
	{
		img.set_attribute
			( "src"
			, isFullScreen
			? L"view-restore.png"
			: L"view-fullscreen.png"
			);
		img.update();
	}
}

//------------------------
// window message handlers
//------------------------

void NoteView::OnClose(Msg<WM_CLOSE> &msg)
{
	Hide();
	msg.handled_ = true;
}

void NoteView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteView::OnClose,
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

void NoteView::OnFullScreen(BEHAVIOR_EVENT_PARAMS * params)
{
	ToggleFullScreen();
}

void NoteView::OnHome(BEHAVIOR_EVENT_PARAMS * params)
{
	CloseWindow(hwnd_);
}

void NoteView::OnInput(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	if (e.get_value().get(false))
		e.set_attribute("checked", L"");
	else
		e.remove_attribute("checked");
	isDirty = true;
}

BOOL NoteView::OnLoadData(NMHL_LOAD_DATA * params)
{
	if (NULL == wcschr(params->uri, L':'))
		return __super::OnLoadData(params);
	try
	{
		SignalLoadingData(params->uri, blob);
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		return LOAD_DISCARD;
	}
	params->outData     = &blob[0];
	params->outDataSize = blob.size();
	return LOAD_OK;
}
