#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	bool         isShown;

	DataSignal SignalLoadingData;

public:

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void Hide();

	virtual void SetBody(std::wstring html);

	virtual void SetSubtitle(std::wstring html);

	virtual void SetTitle(std::wstring html);

	virtual void Show();
};
