#pragma once
#include "ISignalProvider.h"

#include "Blob.h"
#include "InkPen.h"

class IInkEditorView : public ISignalProvider
{
	MacroIEvent(Accept)
	MacroIEvent(Cancel)
	MacroIEvent(PenChanged)

public:

	virtual void GetImage(Blob & image) = 0;

	virtual InkPenColor GetPenColor() = 0;

	virtual InkPenWidth GetPenWidth() = 0;

	virtual void Hide() = 0;

	virtual void SetPen(InkPenWidth width, InkPenColor color) = 0;

	virtual void Show() = 0;
};
