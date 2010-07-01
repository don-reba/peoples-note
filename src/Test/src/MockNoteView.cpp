#include "stdafx.h"
#include "MockNoteView.h"

#include <algorithm>

using namespace std;

MockNoteView::MockNoteView()
	: isDirty (false)
	, isShown (false)
{
}

void MockNoteView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void MockNoteView::ConnectEdit(slot_type OnEdit)
{
	SignalEdit.connect(OnEdit);
}

void MockNoteView::ConnectLoadingData(DataSlot OnLoadingData)
{
	SignalLoadingData.connect(OnLoadingData);
}

void MockNoteView::GetBody(wstring & html)
{
	html = body;
}

void MockNoteView::GetTitle(std::wstring & text)
{
	text = title;
}

void MockNoteView::Hide()
{
	isShown = false;
}

bool MockNoteView::IsDirty()
{
	return isDirty;
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

