#include "stdafx.h"
#include "MockPhotoEditorView.h"

using namespace std;

MockPhotoEditorView::MockPhotoEditorView()
	: isShown     (false)
	, photoResult (0)
{
}
int MockPhotoEditorView::CapturePhoto
	( int            quality
	, int            width
	, int            height
	, std::wstring & path
	)
{
	photoQuality = quality;
	photoWidth   = width;
	photoHeight  = height;
	path = this->path;
	return photoResult;
}

void MockPhotoEditorView::ConnectCancel(slot_type OnCancel)
{
	SignalCancel.connect(OnCancel);
}

void MockPhotoEditorView::ConnectOk(slot_type OnOk)
{
	SignalOk.connect(OnOk);
}

void MockPhotoEditorView::Hide()
{
	isShown = false;
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

void MockPhotoEditorView::SetMessage(const wstring & message)
{
	this->message = message;
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
