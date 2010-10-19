#pragma once
#include "ISignalProvider.h"

#include "Blob.h"
#include "InkPen.h"

class IInkEditorView : public ISignalProvider
{
public:

	virtual void ConnectAccept(slot_type OnAccept) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectPenChanged(slot_type OnPenChanged) = 0;

	virtual void GetImage(Blob & image) = 0;

	virtual InkPenColor GetPenColor() = 0;

	virtual InkPenWidth GetPenWidth() = 0;

	virtual void Hide() = 0;

	virtual void SetPen(InkPenWidth width, InkPenColor color) = 0;

	virtual void Show() = 0;
};
