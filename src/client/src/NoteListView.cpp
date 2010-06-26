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
	, sipState        (SHFS_HIDESIPBUTTON)
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

	::ShowWindow(hwnd_, cmdShow);
	::UpdateWindow(hwnd_);
}

void NoteListView::RegisterEventHandlers()
{
	ConnectBehavior("#menu-about",    MENU_ITEM_CLICK,          &NoteListView::OnMenuAbout);
	ConnectBehavior("#menu-exit",     MENU_ITEM_CLICK,          &NoteListView::OnMenuExit);
	ConnectBehavior("#menu-import",   MENU_ITEM_CLICK,          &NoteListView::OnMenuImport);
	ConnectBehavior("#menu-signin",   MENU_ITEM_CLICK,          &NoteListView::OnMenuSignIn);
	ConnectBehavior("#note-list",     SELECT_SELECTION_CHANGED, &NoteListView::OnNote);
	ConnectBehavior("#search-button", BUTTON_CLICK,             &NoteListView::OnSearch);
	ConnectBehavior("#sync-button",   BUTTON_CLICK,             &NoteListView::OnSync);

	noteList     = FindFirstElement("#note-list");
	notebookList = FindFirstElement("#notebook-list");
	listScroll   = FindFirstElement("#scroll");
	listSlider   = FindFirstElement("#slider");
	searchBox    = FindFirstElement("#search-box");
	status       = FindFirstElement("#status");
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

void NoteListView::AddNotebook(wstring html, wstring value)
{
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8 = ConvertToUtf8(html, htmlUtf8Chars);

	element notebook = element::create("li");
	notebookList.append(notebook);
	notebook.set_attribute("guid", value.c_str());
	notebook.set_html(htmlUtf8, htmlUtf8Chars.size());

	ConnectBehavior(static_cast<HELEMENT>(notebook), MENU_ITEM_CLICK, &NoteListView::OnMenuNotebook);
}

void NoteListView::ClearNotebooks()
{
	vector<element> notebooks;
	notebookList.find_all(notebooks, "li");
	foreach (element & notebook, notebooks)
	{
		DisconnectBehavior(notebook);
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

void NoteListView::ConnectAbout(slot_type OnAbout)
{
	SignalAbout.connect(OnAbout);
}

void NoteListView::ConnectImport(slot_type OnImport)
{
	SignalImport.connect(OnImport);
}

void NoteListView::ConnectLoadThumbnail(DataSlot OnLoadThumbnail)
{
	SignalLoadThumbnail.connect(OnLoadThumbnail);
}

void NoteListView::ConnectNotebookSelected(slot_type OnNotebookSelected)
{
	SignalNotebookSelected.connect(OnNotebookSelected);
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

void NoteListView::ConnectSync(slot_type OnSync)
{
	SignalSync.connect(OnSync);
}

void NoteListView::DisableSync()
{
	// TODO: implement
}

void NoteListView::EnableSync()
{
	// TODO: implement
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

Guid NoteListView::GetSelectedNotebookGuid()
{
	return selectedNotebookGuid;
}

Guid NoteListView::GetSelectedNoteGuid()
{
	element root     (element::root_element(hwnd_));
	element noteList (root.find_first("#note-list"));
	return noteList.get_value().to_string().c_str();
}

wstring NoteListView::GetSearchString()
{
	return searchBox.text().c_str();
}

void NoteListView::HideSyncButton()
{
	//element root (element::root_element(hwnd_));
	//element sync (root.find_first("#sync-panel"));
	//if (!sync)
	//	throw std::exception("'#sync-panel' not found.");
	//sync.set_style_attribute("display", L"none");
}

void NoteListView::SetProfileText(const wstring & text)
{
	element root    (element::root_element(hwnd_));
	element profile (root.find_first("#menu-profile"));
	if (!profile)
		throw std::exception("'#menu-profile' not found.");
	profile.set_text(text.c_str());
}

void NoteListView::SetSigninText(const wstring & text)
{
	element root   (element::root_element(hwnd_));
	element signin (root.find_first("#menu-signin"));
	if (!signin)
		throw std::exception("'#menu-signin' not found.");
	signin.set_text(text.c_str());
}

void NoteListView::ShowSyncButton()
{
	element root (element::root_element(hwnd_));
	element sync (root.find_first("#sync-panel"));
	if (!sync)
		throw std::exception("'#sync-panel' not found.");
	sync.set_style_attribute("display", L"block");
}

void NoteListView::SetStatusText(const wstring  & text)
{
	status.set_text(text.c_str());
}

void NoteListView::SetSyncText(const wstring & text)
{
	element root (element::root_element(hwnd_));
	element sync (root.find_first("#sync-text"));
	if (!sync)
		throw std::exception("'#sync-text' not found.");
	sync.set_text(text.c_str());
}

void NoteListView::SetWindowTitle(const wstring & text)
{
	::SetWindowText(hwnd_, text.c_str());
}

void NoteListView::UpdateNotebooks()
{
	notebookList.update(true);
}

void NoteListView::UpdateNotes()
{
	noteList.update(MEASURE_DEEP|REDRAW_NOW);
	UpdateScrollbar();
}

void NoteListView::UpdateThumbnail(const Guid & guid)
{
	wstring uri(L"thumb:");
	uri.append(ConvertToUnicode(guid));

	Blob * blob(NULL);
	SignalLoadThumbnail(guid, blob);
	if (blob && !blob->empty())
	{
		HTMLayoutDataReadyAsync
			( hwnd_
			, uri.c_str()
			, &blob->at(0)
			, blob->size()
			, HLRT_DATA_IMAGE
			);
	}
}

//------------------
// utility functions
//------------------

void NoteListView::AnimateScroll(DWORD time)
{
	const int    t   (time);
	const int    sgn ((dragSpeed >= 0.0) ? 1 : -1);
	const double s   (fabs(dragSpeed));
	const double a   (acceleration);
	if (s + a * t > 0.001)
	{
		int offset(sgn * static_cast<int>(t * (s + 0.5 * a * t)));
		SetNoteListScrollPos(startScrollPos - offset);
	}
	else
	{
		state = StateIdle;
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);
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

void NoteListView::SetNoteListScrollPos(int pos)
{
	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	noteList.get_scroll_info(scrollPos, viewRect, contentSize);
	int contentHeight(contentSize.cy);

	RECT listRect(noteList.get_location(SCROLLABLE_AREA));
	int scrollableHeight(listRect.bottom - listRect.top);
	if (scrollableHeight <= 0)
		return;

	int contentDistance(contentHeight - scrollableHeight);
	if (contentDistance <= 0)
		return;

	RECT scrollRect(listScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
	int scrollHeight(scrollRect.bottom - scrollRect.top);
	if (scrollHeight <= 0)
		return;

	RECT sliderRect(listSlider.get_location(CONTAINER_RELATIVE|BORDER_BOX));
	int sliderHeight(sliderRect.bottom - sliderRect.top);
	if (sliderHeight <= 0)
		return;

	int scrollDistance(scrollHeight - sliderHeight);
	if (scrollDistance <= 0)
		return;

	pos = min(max(pos, 0), contentDistance);

	POINT point = { 0, pos };
	noteList.set_scroll_pos(point, false);

	point.y = -static_cast<int>(static_cast<__int64>(pos) * scrollDistance / contentDistance);
	listScroll.set_scroll_pos(point, false);
}

void NoteListView::UpdateScrollbar()
{
	POINT scrollPos;
	RECT  viewRect;
	SIZE  contentSize;
	noteList.get_scroll_info(scrollPos, viewRect, contentSize);
	if (contentSize.cy <= 0)
		return;

	RECT listRect(noteList.get_location(SCROLLABLE_AREA));
	__int64 scrollableHeight(listRect.bottom - listRect.top);
	if (scrollableHeight <= 0L)
		return;

	if (contentSize.cy <= scrollableHeight)
	{
		listSlider.set_style_attribute("display", L"none");
		return;
	}
	else
	{
		listSlider.set_style_attribute("display", L"block");
	}

	RECT scrollRect(listScroll.get_location(ROOT_RELATIVE|CONTENT_BOX));
	__int64 scrollHeight(scrollRect.bottom - scrollRect.top);
	if (scrollHeight <= 0L)
		return;

	__int64 sliderHeight(scrollHeight * scrollableHeight / contentSize.cy);

	wchar_t heightText[16];
	_itow_s(static_cast<int>(sliderHeight), heightText, 16, 10);
	listSlider.set_style_attribute("height", heightText);
}

//------------------------
// window message handlers
//------------------------

void NoteListView::OnActivate(Msg<WM_ACTIVATE> & msg)
{
	if (msg.GetActiveState() != WA_INACTIVE)
	{
		::SHFullScreen(hwnd_, sipState);
		::SHHandleWMActivate(hwnd_, msg.wprm_, msg.lprm_, &activateInfo, 0);
		msg.handled_ = true;
	}
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
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);

	state = StateDragging;
	startTime = ::GetTickCount();

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
		SetNoteListScrollPos(startScrollPos + lButtonDownY - msg.Position().y);

}

void NoteListView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	if (state == StateDragging)
	{
		int distance = msg.Position().y - lButtonDownY;
		//if (4 < abs(distance))
		{
			startScrollPos = GetNoteListScrollPos();

			POINT scrollPos;
			RECT  viewRect;
			SIZE  contentSize;
			noteList.get_scroll_info(scrollPos, viewRect, contentSize);

			state     = StateAnimating;
			//dragSpeed = distance / static_cast<double>(::GetTickCount() - startTime);
			dragSpeed = (startScrollPos < (contentSize.cy + viewRect.top - viewRect.bottom) / 2) ? -1.0 : 1.0;
			animator.Subscribe
				( IAnimator::AnimationNoteListScroll
				, bind(&NoteListView::AnimateScroll, this, _1)
				);
		}
		//else
		//{
			//state = StateIdle;
			//__super::ProcessMessage(*lButtonDown);
			//lButtonDown.reset();
		//}
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

BOOL NoteListView::OnFocus(FOCUS_PARAMS * params)
{
	DWORD oldSipState(sipState);
	sipState
		= (root_element::focus_element(hwnd_) == searchBox)
		? SHFS_SHOWSIPBUTTON
		: SHFS_HIDESIPBUTTON;
	if (oldSipState != sipState)
		::SHFullScreen(hwnd_, sipState);
	return FALSE;
}

BOOL NoteListView::OnLoadData(NMHL_LOAD_DATA * params)
{
	wstring prefix(L"thumb:");
	if (0 == wcsncmp(params->uri, prefix.c_str(), prefix.size()))
	{
		Guid guid(params->uri + prefix.size());
		Blob * blob(NULL);
		SignalLoadThumbnail(guid, blob);
		if (NULL == blob)
			return LOAD_DISCARD;
		params->outData     = &blob->at(0);
		params->outDataSize = blob->size();
		return LOAD_OK;
	}
	return __super::OnLoadData(params);
}

void NoteListView::OnMenuAbout(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalAbout();
}

void NoteListView::OnMenuExit(BEHAVIOR_EVENT_PARAMS * params)
{
	CloseWindow(hwnd_);
}

void NoteListView::OnMenuImport(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalImport();
}

void NoteListView::OnMenuNotebook(BEHAVIOR_EVENT_PARAMS * params)
{
	element notebook(params->heTarget);
	selectedNotebookGuid = Guid(notebook.get_attribute("guid"));
	SignalNotebookSelected();
}

void NoteListView::OnMenuSignIn(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSignIn();
}

void NoteListView::OnNote(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalOpenNote();
}

void NoteListView::OnSearch(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSearch();
}

void NoteListView::OnSync(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSync();
}
