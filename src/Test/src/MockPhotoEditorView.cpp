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

void MockPhotoEditorView::InitiateCapture()
{
	isCaptureInitiated = true;
}

wstring MockPhotoEditorView::GetImagePath()
{
	return imagePath;
}

wstring MockPhotoEditorView::GetTitle()
{
	return title;
}

void MockPhotoEditorView::Show()
{
	isShown = true;
}
