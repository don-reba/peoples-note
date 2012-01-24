#include "stdafx.h"
#include "HtmlEditor.h"

using namespace htmlayout::dom;
using namespace htmlayout;
using namespace std;

HtmlEditor::HtmlEditor()
	: hwnd(NULL)
{
	HINSTANCE instance(::GetModuleHandle(NULL));

	wchar_t * className  (L"TEST_EDITOR");
	wchar_t * windowName (L"Test Editor");

	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &HtmlEditor::WndProc;
	wc.hInstance     = instance;
	wc.lpszClassName = className;
	if (!::RegisterClass(&wc))
		throw std::exception("Class registration failed.");

	DWORD windowStyle(WS_POPUP|WS_VISIBLE);

	hwnd = ::CreateWindow
		( className        // lpClassName
		, windowName       // lpWindowName
		, windowStyle      // dwStyle
		, CW_USEDEFAULT    // x
		, CW_USEDEFAULT    // y
		, CW_USEDEFAULT    // nWidth
		, CW_USEDEFAULT    // nHeight
		, NULL             // hWndParent
		, NULL             // hMenu
		, instance         // hInstance
		, NULL             // lpParam
		);
	if (!hwnd)
		throw std::exception("Window creation failed.");
}

HtmlEditor::~HtmlEditor()
{
	if (hwnd)
		::DestroyWindow(hwnd);
}

void HtmlEditor::SetHtml(const wchar_t * html)
{
	element root  (element::root_element(hwnd));
	element field (root.find_first("richtext"));

	field.set_value(json::value::from_string(html));
}

void HtmlEditor::GetHtml(wstring & html)
{
	element root  (element::root_element(hwnd));
	element field (root.find_first("richtext"));

	html = field.get_value().to_string();
}


LRESULT CALLBACK HtmlEditor::ProcessHTMLayoutNotify
	( UINT   uMsg
	, WPARAM wParam
	, LPARAM lParam
	, LPVOID vParam
	)
{
	return 0;
}

LRESULT CALLBACK HtmlEditor::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL handled;
	LRESULT result(HTMLayoutProcND(hWnd, uMsg, wParam, lParam, &handled));
	if (handled)
		return result;

	if (uMsg == WM_CREATE)
	{
		//HTMLayoutSetCallback
		//	( hWnd
		//	, &HtmlEditor::ProcessHTMLayoutNotify
		//	, NULL
		//	);

		const char * html("<html><body><richtext style='width:100%%;height:100%%' /></body></html>");
		if (!HTMLayoutLoadHtml(hWnd, reinterpret_cast<LPCBYTE>(html), strlen(html)))
			throw exception("Failed to load interface.");

		return 0;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
