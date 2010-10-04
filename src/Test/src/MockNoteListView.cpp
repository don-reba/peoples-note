#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated              (true)
	, isPageDownVisible         (false)
	, isPageUpVisible           (false)
	, isSearchButtonSetToSearch (true)
	, isSyncButtonShown         (false)
	, isSyncEnabled             (true)
{
}

void MockNoteListView::AddNote
	( const wstring & html
	, const wstring & value
	)
{
	notes.push_back(NoteRecord());
	notes.back().html  = html;
	notes.back().value = value;
	notesUpdated = false;
}

void MockNoteListView::ClearNotes()
{
	notes.clear();
	notesUpdated = false;
}

void MockNoteListView::ConnectAbout(slot_type OnAbout)
{
	SignalAbout.connect(OnAbout);
}

void MockNoteListView::ConnectClearSearch(slot_type OnClearSearch)
{
	SignalClearSearch.connect(OnClearSearch);
}

void MockNoteListView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void MockNoteListView::ConnectImport(slot_type OnImport)
{
	SignalImport.connect(OnImport);
}

void MockNoteListView::ConnectNewNote(slot_type OnNewNote)
{
	SignalNewNote.connect(OnNewNote);
}

void MockNoteListView::ConnectNotebookSelected(slot_type OnNotebookSelected)
{
	SignalNotebookSelected.connect(OnNotebookSelected);
}

void MockNoteListView::ConnectOpenNote(slot_type OnOpenNote)
{
	SignalOpenNote.connect(OnOpenNote);
}

void MockNoteListView::ConnectPageDown(slot_type OnPageDown)
{
	SignalPageDown.connect(OnPageDown);
}

void MockNoteListView::ConnectPageUp(slot_type OnPageUp)
{
	SignalPageUp.connect(OnPageUp);
}

void MockNoteListView::ConnectProfile(slot_type OnProfile)
{
	SignalProfile.connect(OnProfile);
}

void MockNoteListView::ConnectSearch(slot_type OnSearch)
{
	SignalSearch.connect(OnSearch);
}

void MockNoteListView::ConnectSearchChanged(slot_type OnSearchChanged)
{
	SignalSearchChanged.connect(OnSearchChanged);
}

void MockNoteListView::ConnectSignIn(slot_type OnSignIn)
{
	SignalSignIn.connect(OnSignIn);
}

void MockNoteListView::ConnectSync(slot_type OnSync)
{
	SignalSync.connect(OnSync);
}

void MockNoteListView::DisableSync()
{
	isSyncEnabled = false;
}

void MockNoteListView::EnableSync()
{
	isSyncEnabled = true;
}

bool MockNoteListView::GetEnexPath(std::wstring & path)
{
	if (hasEnexPath)
		path = enexPath;
	return hasEnexPath;
}

Guid MockNoteListView::GetSelectedNoteGuid()
{
	return selectedNoteGuid;
}

Guid MockNoteListView::GetSelectedNotebookGuid()
{
	return selectedNotebookGuid;
}

std::wstring MockNoteListView::GetSearchString()
{
	return searchString;
}

void MockNoteListView::GetThumbSize(SIZE & size)
{
	size = thumbSize;
}

void MockNoteListView::HidePageDown()
{
	isPageDownVisible = false;
}

void MockNoteListView::HidePageUp()
{
	isPageUpVisible = false;
}

void MockNoteListView::HideSyncButton()
{
	isSyncButtonShown = false;
}

void MockNoteListView::SetNotebookMenu(const wstring & html)
{
	notebookMenu = html;
}

void MockNoteListView::SetProfileText(const wstring & text)
{
	profileText = text;
}

void MockNoteListView::SetProgress(double fraction)
{
	progressFraction = fraction;
}

void MockNoteListView::SetSearchButtonToClear()
{
	isSearchButtonSetToSearch = false;
}

void MockNoteListView::SetSearchButtonToSearch()
{
	isSearchButtonSetToSearch = true;
}

void MockNoteListView::SetSearchText(const wstring & text)
{
	searchText = text;
}

void MockNoteListView::SetSigninText(const wstring & text)
{
	signinText = text;
}

void MockNoteListView::SetStatusText(const std::wstring & text)
{
	statusText = text;
}

void MockNoteListView::SetSyncText(const wstring & text)
{
	syncText = text;
}

void MockNoteListView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteListView::ShowPageDown()
{
	isPageDownVisible = true;
}

void MockNoteListView::ShowPageUp()
{
	isPageUpVisible = true;
}

void MockNoteListView::ShowSyncButton()
{
	isSyncButtonShown = true;
}

void MockNoteListView::UpdateNotes()
{
	notesUpdated = true;
}

void MockNoteListView::UpdateThumbnail(const Guid & guid)
{
	updatedThumbnails.insert(guid);
}
