#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	std::wstring windowTitle;
	bool         isShown;

	SIZE renderSize;

	DataSignal SignalLoadingData;

public:

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void Hide();

	virtual void Render(Thumbnail & thumbnail);

	virtual void SetBody(const std::wstring & html);

	virtual void SetSubtitle(const std::wstring & html);

	virtual void SetTitle(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();
};
