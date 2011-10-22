#include "stdafx.h"
#include "NoteTagListModel.h"

#include <algorithm>

using namespace std;

NoteTagListModel::NoteTagListModel()
	: isDirty (false)
{
}

void NoteTagListModel::ClearTag(const wstring & name)
{
	MoveTag(selected, cleared, name);
	isDirty = true;
	SignalTagsChanged();
}

void NoteTagListModel::Commit()
{
	SignalCommitted();
}

const TagList & NoteTagListModel::GetClearedTags()
{
	return cleared;
}

const TagList & NoteTagListModel::GetSelectedTags()
{
	return selected;
}

bool NoteTagListModel::IsDirty()
{
	return isDirty;
}

void NoteTagListModel::SelectTag(const wstring & name)
{
	MoveTag(cleared, selected, name);
	isDirty = true;
	SignalTagsChanged();
}

void NoteTagListModel::SetTags
	( const TagList & cleared
	, const TagList & selected
	)
{
	typedef TagList::iterator Iter;

	this->cleared.assign(cleared.begin(), cleared.end());
	sort(this->cleared.begin(), this->cleared.end(), TagLessThanByName);

	this->selected.assign(selected.begin(), selected.end());
	sort(this->selected.begin(), this->selected.end(), TagLessThanByName);

	foreach (const Tag & tag, this->selected)
	{
		Iter i(lower_bound(this->cleared.begin(), this->cleared.end(), tag, TagLessThanByName));
		if (i != this->cleared.end() && !TagLessThanByName(tag, *i))
			this->cleared.erase(i);
	}

	isDirty = false;
	SignalTagsChanged();
}

void NoteTagListModel::MoveTag
	(       TagList & from
	,       TagList & to
	, const wstring & name
	)
{
	typedef TagList::iterator Iter;

	Tag tag;
	tag.name = name;

	Iter fI(lower_bound(from.begin(), from.end(), tag, TagLessThanByName));
	Iter tI(upper_bound(to.begin(),   to.end(),   tag, TagLessThanByName));
	if (fI != from.end() && !TagLessThanByName(tag, *fI))
	{
		to.insert(tI, *fI);
		from.erase(fI);
	}
}

bool NoteTagListModel::TagLessThanByName(const Tag & tag1, const Tag & tag2)
{
	return tag1.name < tag2.name;
}
