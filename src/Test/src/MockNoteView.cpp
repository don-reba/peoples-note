#include "stdafx.h"
#include "MockNoteView.h"

using namespace std;

void MockNoteView::Hide()
{
	isShown = false;
}

void MockNoteView::SetBody(wstring html)
{
	body = html;
}

void MockNoteView::SetTitle(wstring text)
{
	title = text;
}

void MockNoteView::Show()
{
	isShown = true;
}

