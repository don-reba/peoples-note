#pragma once

#include "Blob.h"
#include "ISignalProvider.h"
#include "Thumbnail.h"

class Note;

class INoteView : public ISignalProvider
{
	MacroIEvent(Close)
	MacroIEvent(Edit)
	MacroIEvent(ToggleMaximize)

public:

	virtual void GetBody(std::wstring & html) = 0;

	virtual void GetNote(Note & note) = 0;

	virtual void GetTitle(std::wstring & text) = 0;

	virtual void Hide() = 0;

	virtual bool IsDirty() = 0;

	virtual bool IsMaximized() = 0;

	virtual void MaximizeWindow() = 0;

	virtual void Render(Thumbnail & thumbnail) = 0;

	virtual void RestoreWindow() = 0;

	virtual void SetNote
		( const Note         & note
		, const std::wstring & titleText
		, const std::wstring & subtitleText
		, const std::wstring & bodyHtml
		, const std::wstring & attachment
		, const bool           enableChrome
		) = 0;

	virtual void SetWindowTitle(const std::wstring & text) = 0;

	virtual void Show() = 0;
};
