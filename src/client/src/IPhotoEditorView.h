#pragma once
#include "ISignalProvider.h"

#include "Blob.h"
#include "PhotoSettings.h"

class IPhotoEditorView : public ISignalProvider
{
public:

	virtual void ConnectCapture(slot_type OnCapture) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectOk(slot_type OnOk) = 0;

	virtual std::wstring GetImagePath() = 0;

	virtual PhotoQuality GetQuality() = 0;

	virtual PhotoResolution GetResolution() = 0;

	virtual std::wstring GetTitle() = 0;

	virtual void Hide() = 0;

	virtual void InitiateCapture() = 0;

	virtual void SetQuality(PhotoQuality quality) = 0;

	virtual void SetResolution(PhotoResolution resolution) = 0;

	virtual void Show() = 0;
};
