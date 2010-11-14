#include "stdafx.h"
#include "MockPhotoEditorView.h"

using namespace std;

MockPhotoEditorView::MockPhotoEditorView()
	: isCaptureInitiated (false)
	, isShown            (false)
{
}

void MockPhotoEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockPhotoEditorView::ConnectOk(slot_type OnOk)
{
	SignalOk.connect(OnOk);
}

void MockPhotoEditorView::ConnectCapture(slot_type OnCapture)
{
	SignalCapture.connect(OnCapture);
}

void MockPhotoEditorView::Hide()
{
	isShown = false;
}

wstring MockPhotoEditorView::GetImagePath()
{
	return imagePath;
}

PhotoQuality MockPhotoEditorView::GetQuality()
{
	return quality;
}

PhotoResolution MockPhotoEditorView::GetResolution()
{
	return resolution;
}

wstring MockPhotoEditorView::GetTitle()
{
	return title;
}

void MockPhotoEditorView::InitiateCapture()
{
	isCaptureInitiated = true;
}

void MockPhotoEditorView::SetQuality(PhotoQuality quality)
{
	this->quality = quality;
}

void MockPhotoEditorView::SetResolution(PhotoResolution resolution)
{
	this->resolution = resolution;
}

void MockPhotoEditorView::Show()
{
	isShown = true;
}
