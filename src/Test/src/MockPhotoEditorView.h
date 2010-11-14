#pragma once
#include "IPhotoEditorView.h"

class MockPhotoEditorView : public IPhotoEditorView
{
public:

	std::wstring imagePath;
	std::wstring title;

	PhotoQuality    quality;
	PhotoResolution resolution;

	bool isCaptureInitiated;
	bool isShown;

	signal SignalCancel;
	signal SignalOk;
	signal SignalCapture;

public:

	MockPhotoEditorView();

public:

	virtual void ConnectCapture(slot_type OnCapture);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectOk(slot_type OnOk);

	virtual std::wstring GetImagePath();

	virtual PhotoQuality GetQuality();

	virtual PhotoResolution GetResolution();

	virtual std::wstring GetTitle();

	virtual void Hide();

	virtual void InitiateCapture();

	virtual void SetQuality(PhotoQuality quality);

	virtual void SetResolution(PhotoResolution resolution);

	virtual void Show();
};
