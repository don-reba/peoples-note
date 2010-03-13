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
	ConnectBehavior("#home", BUTTON_CLICK, &NoteView::OnHome);
}

void NoteView::Create(HWND parent)
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	if (!RegisterClass(wndClass))
		throw std::exception("Class could not be registered.");

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, wndTitle.c_str() // lpWindowName
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

void NoteView::ConnectLoadingData(DataSlot OnLoadingData)
{
	SignalLoadingData.connect(OnLoadingData);
}

void NoteView::Hide()
{
	::ShowWindow(hwnd_, SW_HIDE);
}

void NoteView::SetBody(wstring html)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element body = root.find_first("#body");
	if (!body)
		throw std::exception("#body not found.");

	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = Tools::ConvertToUtf8(html, htmlUtf8Chars);

	body.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteView::SetSubtitle(wstring text)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element body = root.find_first("#subtitle");
	if (!body)
		throw std::exception("#subtitle not found.");
	body.set_text(text.c_str(), text.size());
}

void NoteView::SetTitle(wstring text)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element body = root.find_first("#title");
	if (!body)
		throw std::exception("#title not found.");
	body.set_text(text.c_str(), text.size());
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
	if (!Handler::Call(mmp, this, msg))
		__super::ProcessMessage(msg);
}

//---------------------------
// HTMLayout message handlers
//---------------------------

void NoteView::OnHome()
{
	CloseWindow(hwnd_);
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
#ifdef _DEBUG
		::MessageBox
			( hwnd_
			, ConvertToUnicode(e.what()).c_str()
			, L"Error"
			, MB_ICONERROR | MB_OK
			);
#else
		return LOAD_DISCARD;
#endif
	}
	params->outData     = &blob[0];
	params->outDataSize = blob.size();
	return LOAD_OK;
}
