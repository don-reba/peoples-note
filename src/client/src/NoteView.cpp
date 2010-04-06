#include "stdafx.h"
#include "NoteView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

#include "imaging.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteView::NoteView(HINSTANCE instance)
	: instance        (instance)
	, isFullScreen    (false)
	, HTMLayoutWindow (L"note-view.htm")
{
}

void NoteView::Create(HWND parent)
{
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

	this->parent = parent;

	CopyParentSize();
}

void NoteView::RegisterEventHandlers()
{
	ConnectBehavior("#home",        BUTTON_CLICK, &NoteView::OnHome);
	ConnectBehavior("#full-screen", BUTTON_CLICK, &NoteView::OnFullScreen);
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
	::EnableWindow(parent, TRUE);
	::ShowWindow(hwnd_, SW_HIDE);
}

struct BITMAPINFO_BF
{
	BITMAPINFOHEADER bmiHeader;
	DWORD bmiColorsR;
	DWORD bmiColorsG;
	DWORD bmiColorsB;

	BITMAPINFO * GetBitmapInfo()
	{
		return reinterpret_cast<BITMAPINFO*>(this);
	}
};

HBITMAP NoteView::Render(SIZE size)
{
 	BITMAPINFO_BF info = { 0 };
	info.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth        = size.cx;
	info.bmiHeader.biHeight       = size.cy;
	info.bmiHeader.biPlanes       = 1;
	info.bmiHeader.biBitCount     = 16;
	info.bmiHeader.biCompression  = BI_BITFIELDS;
	info.bmiHeader.biSizeImage    = ((size.cx * 2 + 3) & ~3) * size.cy;
	info.bmiHeader.biClrUsed      = 1;
	info.bmiHeader.biClrImportant = 0;
	info.bmiColorsR               = 0xf800;
	info.bmiColorsG               = 0x07e0;
	info.bmiColorsB               = 0x001f;

	HDC     dc  (::CreateCompatibleDC(::GetDC(hwnd_)));
	HBITMAP bmp (::CreateDIBSection(dc, info.GetBitmapInfo(), DIB_RGB_COLORS, NULL, NULL, 0));
	if (bmp)
	{
		RECT rect = { 0, 0, size.cx, size.cy };
		if (!HTMLayoutRender(hwnd_, bmp, rect))
			throw std::exception("Note rendering failed.");
	}

	//vector<WORD> bits(info.bmiHeader.biSizeImage / 2);
	//::GetDIBits(dc, bmp, 0, size.cy, &bits[0], &info, DIB_RGB_COLORS);

	::DeleteDC(dc);
	return bmp;
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
	root.update(true);
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
	::EnableWindow(parent, FALSE);
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
	dom::element root = dom::element::root_element(hwnd_);
	dom::element img  = root.find_first("#full-screen img");
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

void NoteView::OnFullScreen()
{
	ToggleFullScreen();
}

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
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		return LOAD_DISCARD;
	}
	params->outData     = &blob[0];
	params->outDataSize = blob.size();
	return LOAD_OK;
}
