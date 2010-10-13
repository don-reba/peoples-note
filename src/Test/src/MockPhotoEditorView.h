#pragma once
#include "IPhotoEditorView.h"

class MockPhotoEditorView : public IPhotoEditorView
{
public:

	Blob image;
	std::wstring title;

	bool isCaptureInitiated;
	bool isShown;

	signal SignalCancel;
	signal SignalOk;
	signal SignalCapture;

public:

	MockPhotoEditorView();

public:

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectOk(slot_type OnOk);

	virtual void ConnectCapture(slot_type OnCapture);

	virtual void Hide();

	virtual void InitiateCapture();

	virtual void GetImage(Blob & blob);

	virtual std::wstring GetTitle();

	virtual void Show();
};
