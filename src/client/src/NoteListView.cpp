#include "stdafx.h"

#include "crackers.h"
#include "htmlayout.h"
#include "NoteListView.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

NoteListView::NoteListView(HINSTANCE instance, int cmdShow)
	: cmdShow         (cmdShow)
	, instance        (instance)
	, HTMLayoutWindow (IDH_MAIN)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);

	ConnectBehavior("#search-button", BUTTON_CLICK, &NoteListView::OnSearch);
}

void NoteListView::AddNotebook(wstring notebook)
{
	if (!::AppendMenu(notebooksMenu, 0, 0, notebook.c_str()))
		throw exception("Notebook menu entry could not be added.");
}

void NoteListView::AddNote(wstring noteHtml)
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element noteList = root.find_first("#note-list");
	if (!noteList)
		throw exception("'#note-list' not found.");

	vector<unsigned char> noteHtmlUtf8 = Tools::ConvertToUtf8(noteHtml);
	dom::element note = dom::element::create("option");
	noteList.append(note);
	note.set_attribute("class", L"note");
	note.set_html(&noteHtmlUtf8[0], noteHtmlUtf8.size());
}

void NoteListView::ClearNotebooks()
{
	while (RemoveMenu(notebooksMenu, 0, MF_BYPOSITION));
}

void NoteListView::ClearNotes()
{
	dom::element root = dom::element::root_element(hwnd_);
	vector<dom::element> notes;
	root.find_all(notes, "#note-list .note");

	foreach (dom::element & note, notes)
		note.destroy();
}

void NoteListView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void NoteListView::ConnectImport(slot_type OnImport)
{
	SignalImport.connect(OnImport);
}

void NoteListView::ConnectSearch(slot_type OnSearch)
{
	SignalSearch.connect(OnSearch);
}

void NoteListView::Create()
{
	wstring wndTitle = LoadStringResource(IDS_APP_TITLE);
	wstring wndClass = LoadStringResource(IDC_CLIENT);

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
		, NULL             // hWndParent
		, NULL             // hMenu
		, instance         // hInstance
		, this             // lpParam
		);
	if (!hwnd_)
		throw std::exception("Window creation failed.");

	ResizeForMenuBar();

	::ShowWindow(hwnd_, cmdShow);
	::UpdateWindow(hwnd_);
}

bool NoteListView::GetEnexPath(wstring & path)
{
	vector<wchar_t> file(MAX_PATH);
	OPENFILENAME parameters = { sizeof(parameters) };
	parameters.hwndOwner   = hwnd_;
	parameters.lpstrFile   = &file[0];
	parameters.nMaxFile    = file.size();
	parameters.lpstrTitle  = L"Open Evernote notebook";
	parameters.lpstrFilter = L"Evernote notebook (*.enex)\0*.enex\0";
	parameters.Flags
		= OFN_FILEMUSTEXIST
		| OFN_HIDEREADONLY
		;
	if (::GetOpenFileName(&parameters))
	{
		path = &file[0];
		return true;
	}
	return false;
}

wstring NoteListView::GetSearchString()
{
	dom::element root = dom::element::root_element(hwnd_);
	dom::element searchBox = root.find_first("#search-box");
	if (!searchBox)
		throw exception("'#search-box' not found.");
	return searchBox.text().c_str();
}

void NoteListView::UpdateNotebooks()
{
}

void NoteListView::UpdateNotes()
{
	dom::element root = dom::element::root_element(hwnd_);
	root.update();
}

//------------------
// utility functions
//------------------

void NoteListView::CreateMenuBar()
{
	notebooksMenu = NULL;
	menuBar       = NULL;

	SHMENUBARINFO mbi = { sizeof(SHMENUBARINFO) };
	mbi.hwndParent = hwnd_;
	mbi.nToolBarId = IDR_MENU;
	mbi.hInstRes   = instance;
	if (!::SHCreateMenuBar(&mbi))
		throw exception("Menu bar could not be created.");
	menuBar = mbi.hwndMB;

	TBBUTTONINFO buttonInfo = { sizeof(buttonInfo) };
	buttonInfo.dwMask = TBIF_LPARAM;
	if (!::Toolbar_GetButtonInfo(menuBar, IDM_MENU, &buttonInfo))
		throw exception("Menu not found.");
	HMENU menu = reinterpret_cast<HMENU>(buttonInfo.lParam);

	notebooksMenu = ::GetSubMenu(menu, NotebooksMenuIndex);
	if (!notebooksMenu)
		throw exception("Notebook menu not found.");
}

ATOM NoteListView::RegisterClass(wstring wndClass)
{
	WNDCLASS wc = { 0 };
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = &NoteListView::WndProc<NoteListView>;
	wc.hInstance     = instance;
	wc.hIcon         = LoadIcon(instance, MAKEINTRESOURCE(IDI_CLIENT));
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = wndClass.c_str();
	return ::RegisterClass(&wc);
}

void NoteListView::ResizeForMenuBar()
{
	if (menuBar)
	{
		RECT windowRect;
		RECT menuBarRect;

		::GetWindowRect(hwnd_,   &windowRect);
		::GetWindowRect(menuBar, &menuBarRect);
		windowRect.bottom -= (menuBarRect.bottom - menuBarRect.top);

		int  x       = windowRect.left;
		int  y       = windowRect.top;
		int  width   = windowRect.right  - windowRect.left;
		int  height  = windowRect.bottom - windowRect.top;
		BOOL repaint = FALSE;

		::MoveWindow(hwnd_, x, y, width, height, repaint);
	}
}

//------------------------
// window message handlers
//------------------------

void NoteListView::OnActivate(Msg<WM_ACTIVATE> &msg)
{
	SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, FALSE);
	msg.result_  = 0;
	msg.handled_ = true;
}

void NoteListView::OnBehaviorNotify(Msg<WM_BEHAVIOR_NOTIFY> &msg)
{
	msg.handled_ = true;
}

void NoteListView::OnCommand(Msg<WM_COMMAND> &msg)
{
	switch (msg.GetCtrlId())
	{
	case IDM_MENU_IMPORTNOTES:
		SignalImport();
		break;
	case IDM_MENU_ABOUT:
		// TODO: implement About dialog
		break;
	case IDM_MENU_EXIT:
		SendMessage(hwnd_, WM_CLOSE, 0, 0);
		msg.handled_ = true;
		break;
	}
}

void NoteListView::OnCreate(Msg<WM_CREATE> &msg)
{
	CreateMenuBar();
}

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
	CommandBar_Destroy(menuBar);
	PostQuitMessage(0);
}

void NoteListView::OnSettingChange(Msg<WM_SETTINGCHANGE> &msg)
{
	SHHandleWMSettingChange(hwnd_, msg.wprm_, msg.lprm_, &activateInfo);
	msg.handled_ = true;
}

void NoteListView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteListView::OnActivate,
		&NoteListView::OnBehaviorNotify,
		&NoteListView::OnCommand,
		&NoteListView::OnCreate,
		&NoteListView::OnDestroy,
		&NoteListView::OnSettingChange
	};
	try
	{
		if (!Handler::Call(mmp, this, msg))
			__super::ProcessMessage(msg);
	}
	catch(std::exception e)
	{
		MessageBox
			( hwnd_
			, Tools::ConvertToUnicode(e.what()).c_str()
			, L"Error"
			, MB_OK | MB_ICONERROR
			);
	}
}

//---------------------------
// HTMLayout message handlers
//---------------------------

void NoteListView::OnSearch()
{
	SignalSearch();
}
