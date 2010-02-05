#include "stdafx.h"
#include "MockNoteListView.h"

using namespace std;

MockNoteListView::MockNoteListView()
	: updated (true)
{
}

void MockNoteListView::ClearNotes()
{
	notes.clear();
	updated = false;
}

void MockNoteListView::AddNote(wstring note)
{
	notes.push_back(note);
	updated = false;
}

void MockNoteListView::Update()
{
	updated = true;
}
