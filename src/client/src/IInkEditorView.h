#pragma once
#include "ISignalProvider.h"

#include "Blob.h"

class IInkEditorView : public ISignalProvider
{
public:

	enum PenWidth
	{
		Pen1px,
		Pen2px,
		Pen4px,
		Pen8px,
	};

	enum PenColor
	{
		PenBlack,
		PenWhite,
		PenYellow,
		PenRed,
		PenGreen,
		PenBlue,
	};

public:

	virtual void ConnectAccept(slot_type OnAccept) = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectPenChanged(slot_type OnPenChanged) = 0;

	virtual void GetImage(Blob & image) = 0;

	virtual PenColor GetPenColor() = 0;

	virtual PenWidth GetPenWidth() = 0;

	virtual void Hide() = 0;

	virtual void SetPen(PenWidth width, PenColor color) = 0;

	virtual void Show() = 0;
};
