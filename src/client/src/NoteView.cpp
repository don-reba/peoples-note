﻿#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
#include "IAnimator.h"
#include "Rect.h"
#include "resourceppc.h"
#include "Scrolling.h"
#include "Tools.h"

#include <algorithm>
#include <fstream>
#include <iterator>

#include <boost/scope_exit.hpp>

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

	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	Reset();
}

void NoteView::RegisterEventHandlers()
{
	body    = FindFirstElement("#note");
	vScroll = FindFirstElement("#vscroll");
	hScroll = FindFirstElement("#hscroll");
}

//-------------------------
// INoteView implementation
//-------------------------

static void CALLBACK WriteHtml(LPCBYTE utf8, UINT utf8_length, LPVOID param)
{
	wstring * dst(reinterpret_cast<wstring*>(param));
	ConvertToUnicode(utf8, *dst);
}

void NoteView::GetBody(wstring & html)
{
	const bool outerHtml(false);
	HTMLayoutGetElementHtmlCB
		(FindFirstElement("#body"), outerHtml, &WriteHtml, &html);
}

void NoteView::GetDirtyCheckboxIds(set<int> & dirtyCheckboxIds)
{
	dirtyCheckboxIds.clear();
	dirtyCheckboxIds.insert
		( this->dirtyCheckboxIds.begin()
		, this->dirtyCheckboxIds.end()
		);
}

void NoteView::GetNote(Note & note)
{
	note = this->note;
}

wstring NoteView::GetSavePath
	( const wstring & title
	, const wstring & fileName
	, const wstring & directory
	)
{
	vector<wchar_t> fileNameChars(fileName.begin(), fileName.end());
	fileNameChars.resize(MAX_PATH);
	fileNameChars.back() = L'\0';

	vector<wchar_t> titleChars(title.begin(), title.end());
	titleChars.resize(MAX_PATH);
	titleChars.back() = L'\0';

	OPENFILENAME openFileName = { };
	openFileName.lStructSize     = sizeof(openFileName);
	openFileName.lpstrTitle      = &titleChars[0];
	openFileName.nMaxFileTitle   = titleChars.size();
	openFileName.lpstrFile       = &fileNameChars[0];
	openFileName.nMaxFile        = fileNameChars.size();
	openFileName.lpstrInitialDir = directory.c_str();
	openFileName.hwndOwner       = hwnd_;

	BOOL result(::GetSaveFileName(&openFileName));
	if (0 == result || 6 == result)
		return L"";
	return &fileNameChars[0];
}

Guid NoteView::GetSelecteAttachmentGuid()
{
	const wchar_t * value(element(FindFirstElement("#attachments")).get_attribute("value"));
	if (value)
		return Guid(value);
	return Guid::GetEmpty();
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

	Reset();
}

bool NoteView::IsDirty()
{
	return !dirtyCheckboxIds.empty();
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
	( const Note                   & note
	, const wstring                & titleText
	, const wstring                & subtitleText
	, const wstring                & bodyHtml
	, const AttachmentViewInfoList & attachments
	, const bool                     enableChrome
	)
{
	dirtyCheckboxIds.clear();

	this->note = note;

	SetChrome(enableChrome);

	element title    (FindFirstElement("#title"));
	element subtitle (FindFirstElement("#subtitle"));
	element source   (FindFirstElement("#source"));

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
	if (TrySetHtml(utf8, utf8Chars.size()))
	{
		ConnectBehavior("#body input", BUTTON_STATE_CHANGED, &NoteView::OnInput);
		SetAttachments(attachments);
	}
	else
	{
		throw std::exception("Note could not be parsed.");
	}
}

void NoteView::SetSubtitle(const wstring & subtitleText)
{
	element(FindFirstElement("#subtitle"))
		.set_text (subtitleText.c_str(), subtitleText.size());
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

	SetScrollPos(POINT());
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

void NoteView::Reset()
{
	if (hwnd_)
	{
		::DestroyWindow(hwnd_);
		hwnd_ = NULL;
	}

	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

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

void NoteView::SetAttachments(const AttachmentViewInfoList & attachments)
{
	DisconnectBehavior("#attachments *");

	element list(FindFirstElement("#attachments"));

	vector<element> old;
	list.find_all(old, ":root > option");
	foreach (element & e, old)
		e.destroy();

	foreach (const AttachmentViewInfo & attachment, attachments)
	{
		wstring guid(ConvertToUnicode(attachment.Guid).c_str());

		element content (element::create("div"));
		element play    (element::create("img"));

		element option(element::create("option"));
		option.append(content);
		option.append(play);

		list.insert(option, list.children_count() - 1);

		content.set_style_attribute("background-image", attachment.Icon.c_str());
		content.set_attribute("value", guid.c_str());
		content.set_attribute("class", L"content");
		content.set_text(attachment.Text.c_str());

		play.set_attribute("src",   L"play-attachment.png");
		play.set_attribute("value", guid.c_str());
		play.set_attribute("class", L"play");

		ConnectBehavior(content, BUTTON_CLICK, &NoteView::OnAttachment);
		ConnectBehavior(play,    BUTTON_CLICK, &NoteView::OnPlayAttachment);
	}
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

	element body(FindFirstElement("#body"));
	if (enable)
		body.set_attribute("chrome", L"");
	else
		body.remove_attribute("chrome");
}

void NoteView::SetScrollPos(POINT pos)
{
	ScrollHorizontally (body, hScroll, pos.x);
	ScrollVertically   (body, vScroll, pos.y);
}

void NoteView::UpdateScrollbar()
{
	UpdateHorizontalScrollbar (body, hScroll);
	UpdateVerticalScrollbar   (body, vScroll);
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

bool NoteView::TrySetHtml(const unsigned char * text, size_t textLength)
{
	__try
	{
		HTMLayoutSetElementHtml
			( FindFirstElement("#body")
			, text
			, textLength
			, SIH_REPLACE_CONTENT
			);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
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
	case ID_DELETE_NOTE: SignalDelete();         break;
	case ID_EDIT_NOTE:   SignalEdit();           break;
	case ID_EDIT_TAGS:   SignalEditTags();       break;
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
		::NKDbgPrintfW(L"%s\n", ConvertToUnicode(e.what()).c_str());
		throw e;
	}
}

//---------------------------
// HTMLayout message handlers
//---------------------------

void NoteView::OnAttachment(BEHAVIOR_EVENT_PARAMS * params)
{
	const wchar_t * value(element(params->heTarget).get_attribute("value"));
	if (!value)
		return;
	element(FindFirstElement("#attachments")).set_attribute("value", value);
	SignalAttachment();
}

void NoteView::OnInput(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	int id(e.get_attribute_int("uid", -1));
	set<int>::iterator i(dirtyCheckboxIds.find(id));
	if (i == dirtyCheckboxIds.end())
		dirtyCheckboxIds.insert(id);
	else
		dirtyCheckboxIds.erase(i);
}

void NoteView::OnPlayAttachment(BEHAVIOR_EVENT_PARAMS * params)
{
	const wchar_t * value(element(params->heTarget).get_attribute("value"));
	if (!value)
		return;
	element(FindFirstElement("#attachments")).set_attribute("value", value);
	SignalPlayAttachment();
}
