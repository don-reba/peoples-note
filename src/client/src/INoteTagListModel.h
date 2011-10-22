#pragma once
#include "ISignalProvider.h"

#include "Guid.h"
#include "Tag.h"

class INoteTagListModel : public ISignalProvider
{
	MacroIEvent(TagsChanged)
	MacroIEvent(Committed)

public:

	virtual void ClearTag(const std::wstring & tag) = 0;

	virtual void Commit() = 0;

	virtual const TagList & GetClearedTags() = 0;

	virtual const TagList & GetSelectedTags() = 0;

	virtual bool IsDirty() = 0;

	virtual void SelectTag(const std::wstring & tag) = 0;

	virtual void SetTags
		( const TagList & cleared
		, const TagList & selected
		) = 0;
};
