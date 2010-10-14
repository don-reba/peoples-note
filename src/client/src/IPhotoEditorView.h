#pragma once
#include "ISignalProvider.h"

#include "Blob.h"

class IPhotoEditorView : public ISignalProvider
{
public:

	virtual void ConnectCapture(slot_type OnCapture) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectOk(slot_type OnOk) = 0;

	virtual void Hide() = 0;

	virtual void InitiateCapture() = 0;

	virtual std::wstring GetImagePath() = 0;

	virtual std::wstring GetTitle() = 0;

	virtual void Show() = 0;
};
