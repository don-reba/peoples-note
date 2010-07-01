#pragma once
#include "IEditorView.h"

class MockEditorView : public IEditorView
{
public:

	std::wstring body;
	std::wstring title;

	bool isShown;

	signal SignalAccept;
	signal SignalCancel;

public:

	MockEditorView();

public:

	virtual void ConnectAccept(slot_type OnAccept);

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void GetBody(std::wstring & html);

	virtual void GetTitle(std::wstring & text);

	virtual void Hide();

	virtual void SetBody(const std::wstring & html);

	virtual void SetTitle(const std::wstring & text);

	virtual void Show();
};