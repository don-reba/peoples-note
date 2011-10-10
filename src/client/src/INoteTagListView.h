#pragma once
#include "ISignalProvider.h"

#include "Guid.h"
#include "Tag.h"

class INoteTagListView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Ok)
	MacroIEvent(TagCleared)
	MacroIEvent(TagSelected)

public:

	virtual void Hide() = 0;

	virtual std::wstring GetActiveTag() = 0;

	virtual void SetTags
		( const TagList & cleared
		, const TagList & selected
		) = 0;

	virtual void Show() = 0;
};
