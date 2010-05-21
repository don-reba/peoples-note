#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated      (true)
	, notebooksUpdated  (true)
	, isSyncButtonShown (false)
	, isSyncEnabled     (true)
{
}

void MockNoteListView::AddNote(wstring html, wstring value)
{
	notes.push_back(NoteRecord());
	notes.back().html  = html;
	notes.back().value = value;
	notesUpdated = false;
}

void MockNoteListView::AddNotebook(wstring html, wstring value)
{
	notebooks.push_back(NotebookRecord());
	notebooks.back().html  = html;
	notebooks.back().value = value;
	notebooksUpdated = false;
}

void MockNoteListView::ClearNotebooks()
{
	notebooks.clear();
	notebooksUpdated = false;
}

void MockNoteListView::ClearNotes()
{
	notes.clear();
	notesUpdated = false;
}

void MockNoteListView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void MockNoteListView::ConnectImport(slot_type OnImport)
{
	SignalImport.connect(OnImport);
}

void MockNoteListView::ConnectLoadThumbnail(DataSlot OnLoadThumbnail)
{
	SignalLoadThumbnail.connect(OnLoadThumbnail);
}

void MockNoteListView::ConnectNotebookSelected(slot_type OnNotebookSelected)
{
	SignalNotebookSelected.connect(OnNotebookSelected);
}

void MockNoteListView::ConnectOpenNote(slot_type OnOpenNote)
{
	SignalOpenNote.connect(OnOpenNote);
}

void MockNoteListView::ConnectSearch(slot_type OnSearch)
{
	SignalSearch.connect(OnSearch);
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

void MockNoteListView::HideSyncButton()
{
	isSyncButtonShown = false;
}

void MockNoteListView::SetProfileText(const wstring & text)
{
	profileText = text;
}

void MockNoteListView::SetSigninText(const wstring & text)
{
	signinText = text;
}

void MockNoteListView::SetSyncText(const wstring & text)
{
	syncText = text;
}

void MockNoteListView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteListView::ShowSyncButton()
{
	isSyncButtonShown = true;
}

void MockNoteListView::UpdateNotebooks()
{
	notebooksUpdated = true;
}

void MockNoteListView::UpdateNotes()
{
	notesUpdated = true;
}
