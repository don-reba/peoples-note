#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Thumbnail.h"

class INoteView : public ISignalProvider
{
public:

	typedef boost::signals2::signal
		< void(const wchar_t *, Blob &)
		> DataSignal;
	typedef boost::signals2::signal
		< void(const wchar_t *, Blob &)
		>::slot_type DataSlot;

public:

	virtual void ConnectClose(slot_type OnClose) = 0;

	virtual void ConnectLoadingData(DataSlot OnLoadingData) = 0;

	virtual void GetBody(std::wstring & html) = 0;

	virtual void Hide() = 0;

	virtual bool IsDirty() = 0;

	virtual void Render(Thumbnail & thumbnail) = 0;

	virtual void SetBody(const std::wstring & html) = 0;

	virtual void SetSubtitle(const std::wstring & text) = 0;

	virtual void SetTitle(const std::wstring & text) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void Show() = 0;
};
