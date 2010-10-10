#pragma once
#include "ISignalProvider.h"

#include "Blob.h"

class IInkEditorView : public ISignalProvider
{
public:

	virtual void ConnectAccept(slot_type OnAccept) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void GetImage(Blob & blob) = 0;

	virtual void Hide() = 0;

	virtual void Show() = 0;
};
