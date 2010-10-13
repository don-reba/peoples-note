#include "stdafx.h"
#include "MockInkEditorView.h"

void MockInkEditorView::ConnectAccept(slot_type OnAccept)
{
	SignalAccept.connect(OnAccept);
}

void MockInkEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockInkEditorView::GetImage(Blob & image)
{
	image.assign(this->image.begin(), this->image.end());
}

void MockInkEditorView::Hide()
{
	isShown = false;
}

void MockInkEditorView::Show()
{
	isShown = true;
}
