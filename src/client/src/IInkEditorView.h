#pragma once
#include "ISignalProvider.h"

#include "Blob.h"

class IInkEditorView : public ISignalProvider
{
public:

	virtual void ConnectAccept(slot_type OnAccept) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectPenChanged(slot_type OnPenChanged) = 0;

	virtual void GetImage(Blob & image) = 0;

	virtual const wchar_t * GetPenColor() = 0;

	virtual int GetPenWidth() = 0;

	virtual void Hide() = 0;

	virtual void SetPen(int width, COLORREF color) = 0;

	virtual void Show() = 0;
};
