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

void MockNoteView::SetBody(const wstring & html)
{
	body = html;
}

void MockNoteView::SetSubtitle(const wstring & html)
{
	subtitle = html;
}

void MockNoteView::SetTitle(const wstring & html)
{
	title = html;
}

void MockNoteView::SetWindowTitle(const wstring & text)
{
	windowTitle = text;
}

void MockNoteView::Show()
{
	isShown = true;
}

