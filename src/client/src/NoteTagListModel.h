#pragma once
#include "INoteTagListModel.h"

class NoteTagListModel : public INoteTagListModel
{
	MacroEvent(TagsChanged)
	MacroEvent(Committed)

private:

	TagList cleared;
	TagList selected;

	bool isDirty;

public:

	NoteTagListModel();

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

private:

	static void MoveTag
		(       TagList      & from
		,       TagList      & to
		, const std::wstring & name
		);

	static bool TagLessThanByName(const Tag & tag1, const Tag & tag2);
};
