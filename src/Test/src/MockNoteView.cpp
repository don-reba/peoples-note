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

void MockNoteView::Render(Thumbnail & thumbnail)
{
	renderSize.cx = thumbnail.Width;
	renderSize.cy = thumbnail.Height;
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

