#pragma once
#include "IInkEditorView.h"

class MockInkEditorView : public IInkEditorView
{
public:

	bool isShown;

	Blob image;

	InkPenColor penColor;
	InkPenWidth penWidth;

	InkPenColor setPenColor;
	InkPenWidth setPenWidth;

	signal SignalAccept;
	signal SignalCancel;
	signal SignalPenChanged;
	
public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectPenChanged(slot_type OnPenChanged);

	virtual void GetImage(Blob & image);

	virtual InkPenColor GetPenColor();

	virtual InkPenWidth GetPenWidth();

	virtual void Hide();

	virtual void SetPen(InkPenWidth width, InkPenColor color);

	virtual void Show();
};
