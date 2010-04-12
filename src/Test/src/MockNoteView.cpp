#include "stdafx.h"
#include "MockNoteView.h"

using namespace std;

void MockNoteView::ConnectLoadingData(DataSlot OnLoadingData)
{
	SignalLoadingData.connect(OnLoadingData);
}

void MockNoteView::Hide()
{
	isShown = false;
}

void MockNoteView::Render(SIZE size, Blob & thumbnail)
{
	renderSize = size;
}

void MockNoteView::SetBody(wstring html)
{
	body = html;
}

void MockNoteView::SetSubtitle(wstring html)
{
	subtitle = html;
}

void MockNoteView::SetTitle(wstring html)
{
	title = html;
}

void MockNoteView::Show()
{
	isShown = true;
}

