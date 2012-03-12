#include "stdafx.h"
#include "NoteListView.h"

#include "crackers.h"
#include "IHtmlDataLoader.h"
#include "resourceppc.h"
#include "Scrolling.h"
#include "Tools.h"
#include "WinException.h"

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
		throw WinException(::GetLastError(), L"User32.dll", "Class could not be registered.");

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
		throw WinException(::GetLastError(), L"User32.dll", "Window creation failed.");

	notebookMenu = CreateNotebookMenu();
	viewMenu     = CreateViewMenu();
	mainMenu     = CreateMainMenu(notebookMenu, viewMenu);

	::ShowWindow(hwnd_, cmdShow);
	::UpdateWindow(hwnd_);
}

void NoteListView::RegisterEventHandlers()
{
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
	ConnectBehavior("#menu",                BUTTON_CLICK,       &NoteListView::OnMenu);

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

bool NoteListView::GetRequestedNotebookTitleState()
{
	return requestedNotebookTitleState;
}

NotebookViewStyle NoteListView::GetRequestedViewStyle()
{
	return requestedViewStyle;
}

Guid NoteListView::GetSelectedNotebookGuid()
{
	return selectedNotebookGuid;
}

Guid NoteListView::GetSelectedNoteGuid()
{
	return Guid(noteList.get_attribute("value"));
}

wstring NoteListView::GetSearchText()
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
	static_cast<element>(FindFirstElement("#notebook-title"))
		.set_style_attribute("display", L"none");
	::CheckMenuItem(mainMenu, ID_NOTEBOOK_TITLE, MFS_UNCHECKED);
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

void NoteListView::SelectNotebook(const Guid & notebook)
{
	for (int i(0), size(notebookGuids.size()); i != size; ++i)
	{
		::CheckMenuItem
			( notebookMenu
			, i | 0x8000
			, (notebook == notebookGuids[i]) ? MFS_CHECKED : MFS_UNCHECKED
			);
	}
}

void NoteListView::SetNotebookMenu(const NotebookList & notebooks)
{
	MENUITEMINFO info = { sizeof(info) };
	while (::GetMenuItemInfo(notebookMenu, 0, TRUE, &info))
		::RemoveMenu(notebookMenu, 0, MF_BYPOSITION);
	notebookGuids.clear();

	// we pack the notebook number into the low 15 bits of its WORD-sized ID
	for (int i(0), size(min(0x7FFE, notebooks.size())); i != size; ++i)
	{
		const Notebook & notebook(notebooks.at(i));
		::AppendMenu(notebookMenu, MF_STRING, 0x8000 | i, notebook.name.c_str());
		notebookGuids.push_back(notebook.guid);
	}

	::AppendMenu(notebookMenu, MF_STRING, 0x8000 | notebookGuids.size(), L"All Notebooks");
	notebookGuids.push_back(Guid::GetEmpty());
}

void NoteListView::SetProfileText(const wstring & text)
{
	MENUITEMINFO info = { sizeof(info), MIIM_TYPE };
	info.fType      = MFT_STRING;
	info.dwTypeData = const_cast<LPWSTR>(text.c_str());
	::SetMenuItemInfo(mainMenu, ID_PROFILE, FALSE, &info);
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
	MENUITEMINFO info = { sizeof(info), MIIM_TYPE };
	info.fType      = MFT_STRING;
	info.dwTypeData = const_cast<LPWSTR>(text.c_str());
	::SetMenuItemInfo(mainMenu, ID_SIGNIN, FALSE, &info);
}

void NoteListView::SetStatusText(const wstring  & text)
{
	status.set_text(text.c_str());
}

void NoteListView::SetSyncText(const wstring & text)
{
	static_cast<element>(FindFirstElement("#sync-text"))
		.set_text(text.c_str());
}

void NoteListView::SetViewStyle(NotebookViewStyle style)
{
	int             option    (-1);
	const wchar_t * styleName (NULL);

	switch (style)
	{
	case NotebookViewCombined: option = ID_VIEW_COMBINED; styleName = L"combined"; break;
	case NotebookViewTitles:   option = ID_VIEW_TITLES;   styleName = L"titles";   break;
	}

	int options[] = { ID_VIEW_COMBINED, ID_VIEW_TITLES };
	for (int i(0); i != GetArraySize(options); ++i)
		::CheckMenuItem(viewMenu, options[i], options[i] == option ? MFS_CHECKED : MFS_UNCHECKED);
	if (styleName  != NULL)
		static_cast<element>(FindFirstElement("#note-list")).set_attribute("view-style", styleName);

	SetScrollPos(0);
	UpdateScrollbar();
}

void NoteListView::SetWindowTitle(const wstring & text)
{
	static_cast<element>(FindFirstElement("#notebook-title"))
		.set_text(text.c_str());
	::SetWindowText(hwnd_, text.c_str());
}

void NoteListView::ShowNotebookTitle()
{
	static_cast<element>(FindFirstElement("#notebook-title"))
		.set_style_attribute("display", L"block");
	::CheckMenuItem(mainMenu, ID_NOTEBOOK_TITLE, MFS_CHECKED);
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

void NoteListView::UpdateNotes()
{
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
	::HTMLayoutDataReadyAsync
		( hwnd_           // hwnd
		, uri.c_str()     // uri
		, &blob[0]        // data
		, blob.size()     // dataSize
		, HLRT_DATA_IMAGE // dataType
		);
}

//------------------
// utility functions
//------------------

void NoteListView::BeginStandardGestureRecognition(const POINT & position)
{
	SHRGINFO settings = { sizeof(settings) };
	settings.hwndClient = hwnd_;
	settings.ptDown     = position;
	settings.dwFlags    = SHRG_LONGDELAY;
	::SHRecognizeGesture(&settings);
}

HMENU NoteListView::CreateMainMenu(HMENU notebookMenu, HMENU viewMenu)
{
	HMENU menu(::CreatePopupMenu());
	::AppendMenu(menu, MF_STRING,    ID_PROFILE,        L"");
	::AppendMenu(menu, MF_STRING,    ID_SIGNIN,         L"Sign in");
	::AppendMenu(menu, MF_SEPARATOR, 0U,                NULL);
	::AppendMenu(menu, MF_POPUP,     notebookMenu,      L"Notebooks");
	::AppendMenu(menu, MF_POPUP,     viewMenu,          L"View");
	::AppendMenu(menu, MF_STRING,    ID_IMPORT,         L"Import notes");
	::AppendMenu(menu, MF_SEPARATOR, 0U,                NULL);
	::AppendMenu(menu, MF_STRING,    ID_ABOUT,          L"About");
	::AppendMenu(menu, MF_STRING,    ID_EXIT,           L"Exit");
	return menu;
}

HMENU NoteListView::CreateNotebookMenu()
{
	return ::CreatePopupMenu();
}

HMENU NoteListView::CreateViewMenu()
{
	HMENU menu(::CreatePopupMenu());
	::AppendMenu(menu, MF_STRING,    ID_VIEW_COMBINED,  L"Full view");
	::AppendMenu(menu, MF_STRING,    ID_VIEW_TITLES,    L"Compact");
	::AppendMenu(menu, MF_SEPARATOR, 0U,                NULL);
	::AppendMenu(menu, MF_STRING,    ID_NOTEBOOK_TITLE, L"Show header");
	::CheckMenuItem(menu, ID_VIEW_COMBINED, MFS_CHECKED);
	return menu;
}

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

bool NoteListView::IsNoteSelected()
{
	return noteList.get_attribute("value") != NULL;
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
	ScrollVertically(noteList, listScroll, pos);
}

void NoteListView::UpdateScrollbar()
{
	UpdateVerticalScrollbar(noteList, listScroll);
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

void NoteListView::OnCommand(Msg<WM_COMMAND> & msg)
{
	msg.handled_ = true;
	// notebook selection
	if (msg.CtrlId() & 0x8000)
	{
		selectedNotebookGuid = notebookGuids.at(msg.CtrlId() & 0x7FFF);
		SignalNotebookSelected();
		return;
	}
	// other commands
	switch (msg.CtrlId())
	{
	case ID_ABOUT:          SignalAbout();                     return;
	case ID_DELETE_NOTE:    SignalDeleteNote();                return;
	case ID_EXIT:           CloseWindow(hwnd_);                return;
	case ID_IMPORT:         SignalImport();                    return;
	case ID_NOTEBOOK_TITLE: OnNotebookTitle();                 return;
	case ID_PROFILE:        SignalProfile();                   return;
	case ID_SIGNIN:         SignalSignIn();                    return;
	case ID_VIEW_COMBINED:  OnViewStyle(NotebookViewCombined); return;
	case ID_VIEW_TITLES:    OnViewStyle(NotebookViewTitles);   return;
	}
	// unknown command
	msg.handled_ = false;
}

void NoteListView::OnContextMenu(Msg<WM_CONTEXTMENU> & msg)
{
	if (IsNoteSelected())
	{
		HMENU menu(::CreatePopupMenu());
		::AppendMenu(menu, MF_STRING, ID_DELETE_NOTE, L"Delete note");
		::TrackPopupMenuEx(menu, 0, msg.Position().x, msg.Position().y, hwnd_, NULL);
	}
}

void NoteListView::OnDestroy(Msg<WM_DESTROY> &msg)
{
	PostQuitMessage(0);
}

void NoteListView::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	element clickTarget(element::find_element(hwnd_, msg.Position()));
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

	const wchar_t * value(GetChild(noteList, clickTarget).get_attribute("value"));
	if (value)
		noteList.set_attribute("value", value);

	::SetCapture(hwnd_);

	gestureProcessor.OnMouseDown(msg);

	BeginStandardGestureRecognition(msg.Position());
	
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
		&NoteListView::OnCommand,
		&NoteListView::OnContextMenu,
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
		::NKDbgPrintfW(L"%s\n", ConvertToUnicode(e.what()).c_str());
		throw e;
	}
}

void NoteListView::OnNotebookTitle()
{
	MENUITEMINFO info = { sizeof(info), MIIM_STATE };
	if (!::GetMenuItemInfo(mainMenu, ID_NOTEBOOK_TITLE, FALSE, &info))
		throw WinException(::GetLastError(), L"User32.dll", "Notebook title menu item not found.");
	requestedNotebookTitleState = (info.fState & MFS_CHECKED) == 0;
	SignalNotebookTitleStateChanged();
}

void NoteListView::OnViewStyle(NotebookViewStyle style)
{
	requestedViewStyle = style;
	SignalViewStyleChanged();
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

void NoteListView::OnMenu(BEHAVIOR_EVENT_PARAMS * params)
{
	element e(params->heTarget);
	RECT rect(e.get_location(ROOT_RELATIVE|BORDER_BOX));
	POINT location = { rect.right, rect.top };
	::ClientToScreen(hwnd_, &location);

	::TrackPopupMenuEx(mainMenu, TPM_RIGHTALIGN|TPM_BOTTOMALIGN, location.x, location.y, hwnd_, NULL);
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
	if (IsNoteSelected())
		SignalOpenNote();
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
