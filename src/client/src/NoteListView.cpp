#include "stdafx.h"
#include "NoteListView.h"

#include "crackers.h"
#include "resourceppc.h"
#include "Tools.h"

#include <cmath>

using namespace boost;
using namespace htmlayout;
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
	ConnectBehavior("#note-list",     SELECT_SELECTION_CHANGED, &NoteListView::OnNote);
	ConnectBehavior("#search-button", BUTTON_CLICK,             &NoteListView::OnSearch);

	dom::element root(dom::element::root_element(hwnd_));
	noteList = root.find_first("#note-list");
	if (!noteList)
		throw std::exception("#note-list not found.");
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
		throw std::exception("'#search-box' not found.");
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
		noteList.update();
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

bool NoteListView::IsChild(dom::element child, dom::element parent)
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
	using namespace dom;

	element target(element::find_element(hwnd_, msg.Position()));
	if (!IsChild(target, noteList))
		return;

	if (state == StateAnimating)
	{
		animation.disconnect();
		noteList.update();
	}

	state = StateDragging;
	startTime = animator.GetMilliseconds();

	lButtonDown  = make_shared<WndMsg>(WM_LBUTTONDOWN, msg.lprm_, msg.wprm_);
	lButtonDownY = msg.Position().y;

	startScrollPos = GetNoteListScrollPos();

	msg.handled_ = true;
}

void NoteListView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	using namespace dom;

	msg.handled_ = true;

	if (state == StateDragging)
	{
		element target(element::find_element(hwnd_, msg.Position()));
		if (!IsChild(target, noteList))
			return;
		SetNoteListScrollPos(startScrollPos + lButtonDownY - msg.Position().y);
	}

}

void NoteListView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	using namespace dom;

	if (state == StateDragging)
	{
		element target(element::find_element(hwnd_, msg.Position()));
		if (!IsChild(target, noteList))
			return;

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
