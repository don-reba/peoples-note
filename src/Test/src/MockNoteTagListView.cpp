#include "stdafx.h"
#include "MockNoteTagListView.h"

using namespace std;

MockNoteTagListView::MockNoteTagListView()
	: isShown (false)
{
}

void MockNoteTagListView::Hide()
{
	isShown = false;
}

wstring MockNoteTagListView::GetActiveTag()
{
	return activeTag;
}

void MockNoteTagListView::SetTags
	( const TagList & cleared
	, const TagList & selected
	)
{
	selectedTags = selected;
	clearedTags  = cleared;
}

void MockNoteTagListView::Show()
{
	isShown = true;
}
