#include "stdafx.h"
#include "NoteListView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

#include <cmath>

using namespace boost;
using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteListView::NoteListView
	( IAnimator & animator
	, HINSTANCE   instance
	, int         cmdShow
	)
	: animator        (animator)
	, acceleration    (-0.001)
	, cmdShow         (cmdShow)
	, instance        (instance)
	, HTMLayoutWindow (L"main.htm")
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);
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

void NoteListView::RegisterEventHandlers()
{
	ConnectBehavior("#menu-exit",     MENU_ITEM_CLICK,          &NoteListView::OnMenuExit);
	ConnectBehavior("#menu-import",   MENU_ITEM_CLICK,          &NoteListView::OnMenuImport);
	ConnectBehavior("#menu-signin",   MENU_ITEM_CLICK,          &NoteListView::OnMenuSignIn);
	ConnectBehavior("#note-list",     SELECT_SELECTION_CHANGED, &NoteListView::OnNote);
	ConnectBehavior("#search-button", BUTTON_CLICK,             &NoteListView::OnSearch);

	element root(element::root_element(hwnd_));
	noteList = root.find_first("#note-list");
	if (!noteList)
		throw std::exception("#note-list not found.");
	notebookList = root.find_first("#notebook-list");
	if (!notebookList)
		throw std::exception("#notebook-list not found.");
}

//-----------------------------
// INoteListView implementation
//-----------------------------

void NoteListView::AddNote(wstring html, wstring value)
{
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = ConvertToUtf8(html, htmlUtf8Chars);

	element note(element::create("option"));
	noteList.append(note);
	note.set_attribute("class", L"note");
	note.set_attribute("value", value.c_str());
	note.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteListView::AddNotebook(wstring html)
{
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = ConvertToUtf8(html, htmlUtf8Chars);

	element notebook = element::create("li");
	notebookList.append(notebook);
	notebook.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteListView::ClearNotebooks()
{
	vector<element> notebooks;
	notebookList.find_all(notebooks, "li");
	foreach (element & notebook, notebooks)
	{
		notebook.destroy();
	}
}

void NoteListView::ClearNotes()
{
	element root(element::root_element(hwnd_));
	vector<element> notes;
	root.find_all(notes, "#note-list .note");

	foreach (element & note, notes)
		note.destroy();
}

void NoteListView::ConnectImport(slot_type OnImport)
{
	SignalImport.connect(OnImport);
}

void NoteListView::ConnectLoadThumbnail(DataSlot OnLoadThumbnail)
{
	SignalLoadThumbnail.connect(OnLoadThumbnail);
}

void NoteListView::ConnectOpenNote(slot_type OnOpenNote)
{
	SignalOpenNote.connect(OnOpenNote);
}

void NoteListView::ConnectSearch(slot_type OnSearch)
{
	SignalSearch.connect(OnSearch);
}

void NoteListView::ConnectSignIn(slot_type OnSignIn)
{
	SignalSignIn.connect(OnSignIn);
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
	element root(element::root_element(hwnd_));
	element noteList = root.find_first("#note-list");
	return noteList.get_value().to_string().c_str();
}

wstring NoteListView::GetSearchString()
{
	element root(element::root_element(hwnd_));
	element searchBox = root.find_first("#search-box");
	if (!searchBox)
		throw std::exception("'#search-box' not found.");
	return searchBox.text().c_str();
}

void NoteListView::SignIn()
{
	// TODO: implement
}

void NoteListView::UpdateNotebooks()
{
	notebookList.update(true);
}

void NoteListView::UpdateNotes()
{
	noteList.update(true);
}

//------------------
// utility functions
//------------------

void NoteListView::AnimateScroll(DWORD time)
{
	int    t   (time - startTime);
	int    sgn ((dragSpeed >= 0.0) ? 1 : -1);
	double s   (fabs(dragSpeed));
	double a   (acceleration);
	if (s + a * t > 0.001)
	{
		int offset(sgn * static_cast<int>(t * (s + 0.5 * a * t)));
		SetNoteListScrollPos(startScrollPos - offset);
	}
	else
	{
		state = StateIdle;
		animation.disconnect();
	}
}

int NoteListView::GetNoteListScrollPos()
{
	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	noteList.get_scroll_info(scrollPos, viewRect, contentSize);
	return scrollPos.y;
}

bool NoteListView::IsChild(element child, element parent)
{
	if (!parent)
		return false;
	while (child)
	{
		child = child.parent();
		if (child == parent)
			return true;
	}
	return false;
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

void NoteListView::ResetUiSetup()
{
	::SHFullScreen(hwnd_, SHFS_HIDESIPBUTTON);
}

void NoteListView::SetNoteListScrollPos(int pos)
{
	POINT point = { 0, pos };
	noteList.set_scroll_pos(point);
}

//------------------------
// window message handlers
//------------------------

void NoteListView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
	msg.result_  = 0;
	msg.handled_ = true;
}

void NoteListView::OnCaptureChanged(Msg<WM_CAPTURECHANGED> & msg)
{
	msg.handled_ = true;
}

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
	PostQuitMessage(0);
}

void NoteListView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	element target(element::find_element(hwnd_, msg.Position()));
	if (!IsChild(target, noteList))
		return;

	if (state == StateAnimating)
		animation.disconnect();

	state = StateDragging;
	startTime = animator.GetMilliseconds();

	lButtonDown  = make_shared<WndMsg>(WM_LBUTTONDOWN, msg.lprm_, msg.wprm_);
	lButtonDownY = msg.Position().y;

	startScrollPos = GetNoteListScrollPos();

	::SetCapture(hwnd_);

	msg.handled_ = true;
}

void NoteListView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	msg.handled_ = true;

	if (state == StateDragging)
	{
		SetNoteListScrollPos(startScrollPos + lButtonDownY - msg.Position().y);
	}

}

void NoteListView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	if (state == StateDragging)
	{
		int distance = msg.Position().y - lButtonDownY;
		if (4 < abs(distance))
		{
			startScrollPos = GetNoteListScrollPos();

			int time(animator.GetMilliseconds());

			state     = StateAnimating;
			dragSpeed = distance / static_cast<double>(time - startTime);
			startTime = time;
			animation = animator.Subscribe(bind(&NoteListView::AnimateScroll, this, _1));
		}
		else
		{
			state = StateIdle;
			__super::ProcessMessage(*lButtonDown);
			lButtonDown.reset();
		}
	}
}

void NoteListView::ProcessMessage(WndMsg &msg)
{
	static Handler mmp[] =
	{
		&NoteListView::OnActivate,
		&NoteListView::OnCaptureChanged,
		&NoteListView::OnDestroy,
		&NoteListView::OnMouseDown,
		&NoteListView::OnMouseMove,
		&NoteListView::OnMouseUp,
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

BOOL NoteListView::OnLoadData(NMHL_LOAD_DATA * params)
{
	wstring prefix(L"thumb:");
	if (0 == wcsncmp(params->uri, prefix.c_str(), prefix.size()))
	{
		Blob * blob(NULL);
		SignalLoadThumbnail(_wtoi(params->uri + prefix.size()), blob);
		if (NULL == blob)
			return LOAD_DISCARD;
		params->outData     = &blob->at(0);
		params->outDataSize = blob->size();
		return LOAD_OK;
	}
	return __super::OnLoadData(params);
}

void NoteListView::OnMenuExit()
{
	CloseWindow(hwnd_);
}

void NoteListView::OnMenuSignIn()
{
	SignalSignIn();
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
