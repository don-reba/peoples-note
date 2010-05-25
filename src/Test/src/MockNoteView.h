#pragma once

#include "INoteView.h"

class MockNoteView : public INoteView
{
public:

	std::wstring body;
	std::wstring subtitle;
	std::wstring title;
	std::wstring windowTitle;
	bool         isDirty;
	bool         isShown;

	SIZE renderSize;

	signal     SignalClose;
	DataSignal SignalLoadingData;

public:

	MockNoteView();

	virtual void ConnectClose(slot_type OnClose);

	virtual void ConnectLoadingData(DataSlot OnLoadingData);

	virtual void GetBody(std::wstring & html);

	virtual void Hide();

	virtual bool IsDirty();

	virtual void Render(Thumbnail & thumbnail);

	virtual void SetBody(const std::wstring & html);

	virtual void SetSubtitle(const std::wstring & html);

	virtual void SetTitle(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void Show();
};
