#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated     (true)
	, notebooksUpdated (true)
{
}

void MockNoteListView::ConnectCreated(slot_type OnCreated)
{
	SignalCreated.connect(OnCreated);
}

void MockNoteListView::ClearNotes()
{
	notes.clear();
	notesUpdated = false;
}

void MockNoteListView::UpdateNotes()
{
	notesUpdated = true;
}

void MockNoteListView::AddNote(wstring noteHtml)
{
	notes.push_back(noteHtml);
	notesUpdated = false;
}

void MockNoteListView::ClearNotebooks()
{
	notebooks.clear();
	notebooksUpdated = false;
}

void MockNoteListView::AddNotebook(wstring notebook)
{
	notebooks.push_back(notebook);
	notebooksUpdated = false;
}

void MockNoteListView::UpdateNotebooks()
{
	notebooksUpdated = true;
}
