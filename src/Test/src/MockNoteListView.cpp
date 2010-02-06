#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: notesUpdated     (true)
	, notebooksUpdated (true)
{
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

void MockNoteListView::AddNote(wstring note)
{
	notes.push_back(note);
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
