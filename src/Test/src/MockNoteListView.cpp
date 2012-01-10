#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated              (true)
	, isNotebookTitleVisible    (false)
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
	,       bool      isDirty
	)
{
	notes.push_back(NoteRecord());
	notes.back().html    = html;
	notes.back().value   = value;
	notes.back().isDirty = isDirty;
	notesUpdated = false;
}

void MockNoteListView::ClearNotes()
{
	notes.clear();
	notesUpdated = false;
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

bool MockNoteListView::GetRequestedNotebookTitleState()
{
	return requestedNotebookTitleState;
}

NotebookViewStyle MockNoteListView::GetRequestedViewStyle()
{
	return requestedViewStyle;
}

std::wstring MockNoteListView::GetSearchText()
{
	return searchString;
}

Guid MockNoteListView::GetSelectedNoteGuid()
{
	return selectedNoteGuid;
}

Guid MockNoteListView::GetSelectedNotebookGuid()
{
	return selectedNotebookGuid;
}

void MockNoteListView::GetThumbSize(SIZE & size)
{
	size = thumbSize;
}

void MockNoteListView::HideNotebookTitle()
{
	isNotebookTitleVisible = false;
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

void MockNoteListView::SetNotebookMenu(const NotebookList & notebooks)
{
	notebookMenu.assign(notebooks.begin(), notebooks.end());
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

void MockNoteListView::SetViewStyle(NotebookViewStyle style)
{
	viewStyle = style;
}

void MockNoteListView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteListView::ShowNotebookTitle()
{
	isNotebookTitleVisible = true;
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

void MockNoteListView::UpdateThumbnail(const Guid & note)
{
	updatedThumbnails.insert(note);
}
