#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Thumbnail.h"

class Note;

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

	virtual void ConnectEdit(slot_type OnEdit) = 0;

	virtual void ConnectLoadingData(DataSlot OnLoadingData) = 0;

	virtual void GetBody(std::wstring & html) = 0;

	virtual void GetNote(Note & note) = 0;

	virtual void GetTitle(std::wstring & text) = 0;

	virtual void Hide() = 0;

	virtual bool IsDirty() = 0;

	virtual void Render(Thumbnail & thumbnail) = 0;

	virtual void SetNote
		( const Note         & note
		, const std::wstring & titleText
		, const std::wstring & subtitleText
		, const std::wstring & bodyHtml
		) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void Show() = 0;
};
