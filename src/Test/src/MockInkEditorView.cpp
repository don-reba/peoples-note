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

void MockInkEditorView::ConnectPenChanged(slot_type OnPenChanged)
{
	SignalPenChanged.connect(OnPenChanged);
}

void MockInkEditorView::GetImage(Blob & image)
{
	image.assign(this->image.begin(), this->image.end());
}

InkPenColor MockInkEditorView::GetPenColor()
{
	return penColor;
}

InkPenWidth MockInkEditorView::GetPenWidth()
{
	return penWidth;
}

void MockInkEditorView::Hide()
{
	isShown = false;
}

void MockInkEditorView::SetPen(InkPenWidth width, InkPenColor color)
{
	setPenWidth = width;
	setPenColor = color;
}

void MockInkEditorView::Show()
{
	isShown = true;
}
