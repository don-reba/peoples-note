#pragma once
#include "ISignalProvider.h"

#include "Blob.h"
#include "PhotoSettings.h"

class IPhotoEditorView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Ok)

public:

	virtual int CapturePhoto
		( int            quality
		, int            width
		, int            height
		, std::wstring & path
		) = 0;

	virtual PhotoQuality GetQuality() = 0;

	virtual PhotoResolution GetResolution() = 0;

	virtual std::wstring GetTitle() = 0;

	virtual void Hide() = 0;

	virtual void SetMessage(const std::wstring & message) = 0;

	virtual void SetQuality(PhotoQuality quality) = 0;

	virtual void SetResolution(PhotoResolution resolution) = 0;

	virtual void Show() = 0;
};
