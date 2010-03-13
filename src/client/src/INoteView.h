#pragma once

#include "Blob.h"

class INoteView
{
public:

	typedef boost::signals2::signal
		< void(const wchar_t *, Blob &)
		> DataSignal;
	typedef boost::signals2::signal
		< void(const wchar_t *, Blob &)
		>::slot_type DataSlot;

public:

	virtual void ConnectLoadingData(DataSlot OnLoadingData) = 0;

	virtual void Hide() = 0;

	virtual void SetBody(std::wstring html) = 0;

	virtual void SetTitle(std::wstring text) = 0;

	virtual void Show() = 0;
};
