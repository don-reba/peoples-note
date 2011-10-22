#pragma once
#include "INoteTagListModel.h"

#include "Guid.h"
#include "Tag.h"

class MockNoteTagListModel : public INoteTagListModel
{
	MacroTestEvent(TagsChanged)
	MacroTestEvent(Committed)

public:

	TagList      cleared;
	TagList      selected;
	std::wstring clearedTag;
	std::wstring selectedTag;
	bool         isDirty;

public:

	MockNoteTagListModel();

public:

	virtual void ClearTag(const std::wstring & tag);

	virtual void Commit();

	virtual const TagList & GetClearedTags();

	virtual const TagList & GetSelectedTags();

	virtual bool IsDirty();

	virtual void SelectTag(const std::wstring & tag);

	virtual void SetTags
		( const TagList & cleared
		, const TagList & selected
		);
};
