#include "stdafx.h"
#include "NoteListView.h"

#include "crackers.h"
#include "IHtmlDataLoader.h"
#include "resourceppc.h"
#include "Scrollbar.h"
#include "Tools.h"

#include <sstream>

using namespace boost;
using namespace htmlayout;
using namespace htmlayout::dom;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

NoteListView::NoteListView
	( HINSTANCE         instance
	, bool              highRes
	, IAnimator       & animator
	, int               cmdShow
	, IHtmlDataLoader & htmlDataLoader
	)
	: cmdShow           (cmdShow)
	, instance          (instance)
	, gestureProcessor  (animator)
	, searchButtonState (SearchButtonSearch)
	, sipState          (SHFS_HIDESIPBUTTON)
	, HTMLayoutWindow   (L"main.htm", highRes, htmlDataLoader)
{
	::ZeroMemory(&activateInfo, sizeof(activateInfo));
	activateInfo.cbSize = sizeof(activateInfo);

	gestureProcessor.ConnectDelayedMouseDown (bind(&NoteListView::OnDelayedMouseDown, this));
	gestureProcessor.ConnectGestureStart     (bind(&NoteListView::OnGestureStart,     this));
	gestureProcessor.ConnectGestureStep      (bind(&NoteListView::OnGestureStep,      this));
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
	ConnectBehavior("#menu-about",          MENU_ITEM_CLICK,    &NoteListView::OnMenuAbout);
	ConnectBehavior("#menu-exit",           MENU_ITEM_CLICK,    &NoteListView::OnMenuExit);
	ConnectBehavior("#menu-import",         MENU_ITEM_CLICK,    &NoteListView::OnMenuImport);
	ConnectBehavior("#menu-notebook-title", MENU_ITEM_CLICK,    &NoteListView::OnMenuNotebookTitle);
	ConnectBehavior("#menu-profile",        MENU_ITEM_CLICK,    &NoteListView::OnMenuProfile);
	ConnectBehavior("#menu-signin",         MENU_ITEM_CLICK,    &NoteListView::OnMenuSignIn);
	ConnectBehavior("#new-ink",             BUTTON_CLICK,       &NoteListView::OnNewInk);
	ConnectBehavior("#new-photo",           BUTTON_CLICK,       &NoteListView::OnNewPhoto);
	ConnectBehavior("#new-text",            BUTTON_CLICK,       &NoteListView::OnNewText);
	ConnectBehavior("#new-voice",           BUTTON_CLICK,       &NoteListView::OnNewVoice);
	ConnectBehavior("#note-list",           BUTTON_CLICK,       &NoteListView::OnNote);
	ConnectBehavior("#page-down",           BUTTON_CLICK,       &NoteListView::OnPageDown);
	ConnectBehavior("#page-up",             BUTTON_CLICK,       &NoteListView::OnPageUp);
	ConnectBehavior("#search-box",          EDIT_VALUE_CHANGED, &NoteListView::OnSearchChanged);
	ConnectBehavior("#search-button",       BUTTON_CLICK,       &NoteListView::OnSearch);
	ConnectBehavior("#status-button",       BUTTON_CLICK,       &NoteListView::OnStatus);
	ConnectBehavior("#sync-panel",          BUTTON_CLICK,       &NoteListView::OnSync);

	noteList     = FindFirstElement("#note-list");
	listScroll   = FindFirstElement("#scroll");
	searchBox    = FindFirstElement("#search-box");
	status       = FindFirstElement("#status");
}

//-----------------------------
// INoteListView implementation
//-----------------------------

void NoteListView::AddNote
	( const wstring & html
	, const wstring & value
	,       bool      isDirty
	)
{
	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8(ConvertToUtf8(html, htmlUtf8Chars));

	element note(element::create("div"));
	noteList.insert(note, noteList.children_count() - 1);
	note.set_attribute("class", L"note");
	note.set_attribute("value", value.c_str());
	if (isDirty)
		note.set_attribute("dirty", L"");
	note.set_html(htmlUtf8, htmlUtf8Chars.size());
}

void NoteListView::CheckNotebookTitleOption()
{
	element(FindFirstElement("#menu-notebook-title"))
		.set_attribute("checked", L"");
}

void NoteListView::ClearNotes()
{
	element root(element::root_element(hwnd_));
	vector<element> notes;
	root.find_all(notes, "#note-list .note");

	foreach (element & note, notes)
		note.destroy();
}

void NoteListView::DisableSync()
{
	element e(FindFirstElement("#sync-panel"));
	e.set_attribute("disabled", L"true");
}

void NoteListView::EnableSync()
{
	element e(FindFirstElement("#sync-panel"));
	e.set_attribute("disabled", NULL);
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
	return noteList.get_attribute("value");
}

wstring NoteListView::GetSearchString()
{
	return searchBox.text().c_str();
}

void NoteListView::GetThumbSize(SIZE & size)
{
	element e(element::root_element(hwnd_));
	size.cx = e.get_attribute_int("thumb-width");
	size.cy = e.get_attribute_int("thumb-height");
}

void NoteListView::HideNotebookTitle()
{
	element(FindFirstElement("#notebook-title"))
		.set_style_attribute("display", L"none");
}

void NoteListView::HidePageDown()
{
	element(FindFirstElement("#page-down"))
		.set_style_attribute("display", L"none");
}

void NoteListView::HidePageUp()
{
	element(FindFirstElement("#page-up"))
		.set_style_attribute("display", L"none");
}

void NoteListView::HideSyncButton()
{
	element(FindFirstElement("#sync-panel"))
		.set_style_attribute("display", L"none");
}

bool NoteListView::IsNotebookTitleOptionChecked()
{
	return element(FindFirstElement("#menu-notebook-title"))
		.get_attribute("checked") != NULL;
}

void NoteListView::SetNotebookMenu(const std::wstring & html)
{
	element notebookList(FindFirstElement("#notebook-list"));

	vector<element> notebooks;
	notebookList.find_all(notebooks, "li[guid]");
	foreach (element & notebook, notebooks)
		DisconnectBehavior(notebook);

	vector<unsigned char> htmlUtf8Chars;
	const unsigned char * htmlUtf8(ConvertToUtf8(html, htmlUtf8Chars));
	notebookList.set_html(htmlUtf8, htmlUtf8Chars.size());

	notebooks.clear();
	notebookList.find_all(notebooks, "li[guid]");
	foreach (element & notebook, notebooks)
		ConnectBehavior(notebook, MENU_ITEM_CLICK, &NoteListView::OnMenuNotebook);
}

void NoteListView::SetProfileText(const wstring & text)
{
	element(FindFirstElement("#menu-profile"))
		.set_text(text.c_str());
}

void NoteListView::SetProgress(double fraction)
{
	wstringstream stream;
	stream << (100.0 - 100.0 * fraction) << L"%";
	element(FindFirstElement("#status-progress"))
		.set_style_attribute("background-offset-right", stream.str().c_str());
}

void NoteListView::SetSearchButtonToClear()
{
	searchButtonState = SearchButtonClear;
	element(FindFirstElement("#search-button"))
		.set_style_attribute("background-image", L"url(clear.png");
}

void NoteListView::SetSearchButtonToSearch()
{
	searchButtonState = SearchButtonSearch;
	element(FindFirstElement("#search-button"))
		.set_style_attribute("background-image", L"url(search.png");
}

void NoteListView::SetSearchText(const wstring & text)
{
	element(FindFirstElement("#search-box"))
		.set_text(text.c_str());
}

void NoteListView::SetSigninText(const wstring & text)
{
	element(FindFirstElement("#menu-signin"))
		.set_text(text.c_str());
}

void NoteListView::ShowNotebookTitle()
{
	element(FindFirstElement("#notebook-title"))
		.set_style_attribute("display", L"block");
}

void NoteListView::ShowPageDown()
{
	element(FindFirstElement("#page-down"))
		.set_style_attribute("display", L"block");
}

void NoteListView::ShowPageUp()
{
	element(FindFirstElement("#page-up"))
		.set_style_attribute("display", L"block");
}

void NoteListView::ShowSyncButton()
{
	element(FindFirstElement("#sync-panel"))
		.set_style_attribute("display", L"block");
}

void NoteListView::SetStatusText(const wstring  & text)
{
	status.set_text(text.c_str());
}

void NoteListView::SetSyncText(const wstring & text)
{
	element(FindFirstElement("#sync-text"))
		.set_text(text.c_str());
}

void NoteListView::SetWindowTitle(const wstring & text)
{
	element(FindFirstElement("#notebook-title"))
		.set_text(text.c_str());
	::SetWindowText(hwnd_, text.c_str());
}

void NoteListView::UncheckNotebookTitleOption()
{
	element(FindFirstElement("#menu-notebook-title"))
		.remove_attribute("checked");
}

void NoteListView::UpdateNotes()
{
	noteList.update(MEASURE_DEEP|REDRAW_NOW);
	SetScrollPos(0);
	UpdateScrollbar();
}

void NoteListView::UpdateThumbnail(const Guid & note)
{
	wstring uri(L"thumb:");
	uri.append(ConvertToUnicode(note));

	Blob blob;
	htmlDataLoader.LoadFromUri(uri.c_str(), blob);
	if (blob.empty())
		return;
	HTMLayoutDataReadyAsync
		( hwnd_
		, uri.c_str()
		, &blob[0]
		, blob.size()
		, HLRT_DATA_IMAGE
		);
}

//------------------
// utility functions
//------------------

element NoteListView::GetChild(element parent, element descendant)
{
	if (descendant == parent)
		return element();
	while (descendant && descendant.parent() != parent)
		descendant = descendant.parent();
	return descendant;
}

int NoteListView::GetScrollPos()
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

void NoteListView::OnSearch()
{
	switch (searchButtonState)
	{
	case SearchButtonSearch: SignalSearch();      break;
	case SearchButtonClear:  SignalClearSearch(); break;
	}
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

void NoteListView::SetScrollPos(int pos)
{
	noteList.send_event(TOUCH_SCROLL_POS, pos, listScroll);
}

void NoteListView::UpdateScrollbar()
{
	noteList.send_event(TOUCH_SCROLL_UPDATE, 0, listScroll);
}

//-------------------------
// gesture message handlers
//-------------------------

void NoteListView::OnDelayedMouseDown()
{
	__super::ProcessMessage(*gestureProcessor.GetMouseDownMessage());
}

void NoteListView::OnGestureStart()
{
	startScrollPos = GetScrollPos();
}

void NoteListView::OnGestureStep()
{
	SetScrollPos(startScrollPos + gestureProcessor.GetScrollDistance());
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
	clickTarget = element::find_element(hwnd_, msg.Position());
	if (!IsChild(clickTarget, noteList))
	{
		if (clickTarget == noteList)
			noteList.set_state(STATE_FOCUS);
		return;
	}

	noteList.set_state(STATE_FOCUS);

	const wchar_t * type(clickTarget.get_attribute("type"));
	if (type && 0 == wcsicmp(type, L"button"))
		return;

	::SetCapture(hwnd_);

	gestureProcessor.OnMouseDown(msg);
	
	msg.handled_ = true;
}

void NoteListView::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	gestureProcessor.OnMouseMove(msg);

	msg.handled_ = true;
}

void NoteListView::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	::ReleaseCapture();

	gestureProcessor.OnMouseUp(msg);
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
		= (element::focus_element(hwnd_) == searchBox)
		? SHFS_SHOWSIPBUTTON
		: SHFS_HIDESIPBUTTON;
	if (oldSipState != sipState)
		::SHFullScreen(hwnd_, sipState);
	return FALSE;
}

BOOL NoteListView::OnKey(KEY_PARAMS * params)
{
	if (params->cmd & HANDLED)
		return FALSE;
	if (!(params->cmd & KEY_UP))
		return FALSE;
	if (params->key_code == 0x0D && searchBox == params->target)
	{
		OnSearch();
		return TRUE;
	}
	return FALSE;
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

void NoteListView::OnMenuNotebookTitle(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalNotebookTitle();
}

void NoteListView::OnMenuProfile(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalProfile();
}

void NoteListView::OnMenuSignIn(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSignIn();
}

void NoteListView::OnNewInk(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalNewInkNote();
}

void NoteListView::OnNewPhoto(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalNewPhotoNote();
}

void NoteListView::OnNewText(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalNewNote();
}

void NoteListView::OnNewVoice(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalNewVoiceNote();
}

void NoteListView::OnNote(BEHAVIOR_EVENT_PARAMS * params)
{
	if (IsChild(clickTarget, noteList))
	{
		const wchar_t * value(GetChild(noteList, clickTarget).get_attribute("value"));
		if (value)
		{
			noteList.set_attribute("value", value);
			SignalOpenNote();
		}
	}
}

void NoteListView::OnPageDown(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalPageDown();
}

void NoteListView::OnPageUp(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalPageUp();
}

void NoteListView::OnSearch(BEHAVIOR_EVENT_PARAMS * params)
{
	OnSearch();
}

void NoteListView::OnSearchChanged(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSearchChanged();
}

void NoteListView::OnStatus(BEHAVIOR_EVENT_PARAMS * params)
{
	SetStatusText(L"");
}

void NoteListView::OnSync(BEHAVIOR_EVENT_PARAMS * params)
{
	SignalSync();
}
