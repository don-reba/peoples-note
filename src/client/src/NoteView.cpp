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
}

void NoteView::Create(HWND parent)
{
	wstring wndClass = LoadStringResource(IDC_NOTE_VIEW);

	//if (!RegisterClass(wndClass))
	//	throw std::exception("Class could not be registered.");

	hwnd_ = ::CreateWindow
		( wndClass.c_str() // lpClassName
		, L""              // lpWindowName
		, WS_VISIBLE       // dwStyle
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

	//ResizeForMenuBar();
}

void NoteView::Hide()
{
	::ShowWindow(hwnd_, SW_HIDE);
}

void NoteView::Show()
{
	::ShowWindow(hwnd_, SW_SHOW);
	::UpdateWindow(hwnd_);
}

//-------------------------
// INoteView implementation
//-------------------------

void NoteView::SetBody(wstring html)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element body = root.find_first("body");
	if (!body)
		throw std::exception("Html body not found.");

	
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = Tools::ConvertToUtf8(html, htmlUtf8Chars);

	body.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteView::SetTitle(wstring text)
{
	::SetWindowText(hwnd_, text.c_str());
}
