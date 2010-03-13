#include "stdafx.h"
#include "NoteListView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

using namespace htmlayout;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteListView::NoteListView
	( HINSTANCE  instance
	, int        cmdShow
	)
	: cmdShow         (cmdShow)
	, instance        (instance)
	, HTMLayoutWindow (L"main.htm")
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);

	ConnectBehavior("#menu-exit",     MENU_ITEM_CLICK,          &NoteListView::OnMenuExit);
	ConnectBehavior("#menu-import",   MENU_ITEM_CLICK,          &NoteListView::OnMenuImport);
	ConnectBehavior("#note-list",     SELECT_SELECTION_CHANGED, &NoteListView::OnNote);
	ConnectBehavior("#search-button", BUTTON_CLICK,             &NoteListView::OnSearch);
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

	ResetUiSetup();

	::ShowWindow(hwnd_, cmdShow);
	::UpdateWindow(hwnd_);
}

//-----------------------------
// INoteListView implementation
//-----------------------------

void NoteListView::AddNotebook(wstring notebook)
{
	// TODO: implement
}

void NoteListView::AddNote(wstring html, wstring value)
{
	dom::element root(dom::element::root_element(hwnd_));
	dom::element noteList = root.find_first("#note-list");
	if (!noteList)
		throw exception("'#note-list' not found.");

	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = Tools::ConvertToUtf8(html, htmlUtf8Chars);

	dom::element note = dom::element::create("option");
	noteList.append(note);
	note.set_attribute("class", L"note");
	note.set_attribute("value", value.c_str());
	note.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteListView::ClearNotebooks()
{
	// TODO: implement
}

void NoteListView::ClearNotes()
{
	dom::element root(dom::element::root_element(hwnd_));
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

void NoteListView::ConnectOpenNote(slot_type OnOpenNote)
{
	SignalOpenNote.connect(OnOpenNote);
}

void NoteListView::ConnectSearch(slot_type OnSearch)
{
	SignalSearch.connect(OnSearch);
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
		ResetUiSetup();
		path = &file[0];
		return true;
	}
	ResetUiSetup();
	return false;
}

Guid NoteListView::GetSelectedNoteGuid()
{
	dom::element root(dom::element::root_element(hwnd_));
	dom::element noteList = root.find_first("#note-list");
	return noteList.get_value().to_string().c_str();
}

wstring NoteListView::GetSearchString()
{
	dom::element root(dom::element::root_element(hwnd_));
	dom::element searchBox = root.find_first("#search-box");
	if (!searchBox)
		throw exception("'#search-box' not found.");
	return searchBox.text().c_str();
}

void NoteListView::UpdateNotebooks()
{
	// TODO: implement
}

void NoteListView::UpdateNotes()
{
	dom::element root(dom::element::root_element(hwnd_));
	root.update();
}

//------------------
// utility functions
//------------------

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

void NoteListView::ResetUiSetup()
{
	::SHFullScreen(hwnd_, SHFS_HIDESIPBUTTON);
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

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
	PostQuitMessage(0);
}

void NoteListView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteListView::OnActivate,
		&NoteListView::OnDestroy,
	};
	if (!Handler::Call(mmp, this, msg))
		__super::ProcessMessage(msg);
}

//---------------------------
// HTMLayout message handlers
//---------------------------

void NoteListView::OnMenuExit()
{
	CloseWindow(hwnd_);
}

void NoteListView::OnMenuImport()
{
	SignalImport();
}

void NoteListView::OnNote()
{
	SignalOpenNote();
}

void NoteListView::OnSearch()
{
	SignalSearch();
}
