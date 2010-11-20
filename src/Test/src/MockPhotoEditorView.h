#pragma once
#include "IPhotoEditorView.h"

class MockPhotoEditorView : public IPhotoEditorView
{
public:

	std::wstring title;
	std::wstring message;
	std::wstring path;

	PhotoQuality    quality;
	PhotoResolution resolution;

	int photoQuality;
	int photoWidth;
	int photoHeight;
	int photoResult;

	bool isShown;

	signal SignalCancel;
	signal SignalOk;

public:

	MockPhotoEditorView();

public:

	virtual int CapturePhoto
		( int            quality
		, int            width
		, int            height
		, std::wstring & path
		);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectOk(slot_type OnOk);

	virtual PhotoQuality GetQuality();

	virtual PhotoResolution GetResolution();

	virtual std::wstring GetTitle();

	virtual void Hide();

	virtual void SetMessage(const std::wstring & message);

	virtual void SetQuality(PhotoQuality quality);

	virtual void SetResolution(PhotoResolution resolution);

	virtual void Show();
};
