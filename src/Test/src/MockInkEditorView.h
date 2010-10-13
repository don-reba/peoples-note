#pragma once
#include "IInkEditorView.h"

class MockInkEditorView : public IInkEditorView
{
public:

	bool isShown;

	Blob image;

	signal SignalAccept;
	signal SignalCancel;
	
public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void GetImage(Blob & image);

	virtual void Hide();

	virtual void Show();
};
