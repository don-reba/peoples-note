#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated       (true)
	, notebooksUpdated   (true)
	, isProfileTextShown (false)
	, isSyncButtonShown  (false)
{
}

void MockNoteListView::AddNote(wstring html, wstring value)
{
	NoteRecord record;
	record.html  = html;
	record.value = value;
	notes.push_back(record);
	notesUpdated = false;
}

void MockNoteListView::AddNotebook(wstring html)
{
	notebooks.push_back(html);
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

std::wstring MockNoteListView::GetSearchString()
{
	return searchString;
}

void MockNoteListView::HideProfileText()
{
	isProfileTextShown = false;
}

void MockNoteListView::HideSyncButton()
{
	isSyncButtonShown = false;
}

void MockNoteListView::SetSigninText(const wstring & text)
{
	signinText = text;
}

void MockNoteListView::ShowProfileText(const wstring & text)
{
	isProfileTextShown = false;
	profileText = text;
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
