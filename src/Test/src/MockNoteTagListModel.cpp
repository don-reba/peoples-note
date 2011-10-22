#include "stdafx.h"
#include "MockNoteTagListModel.h"

using namespace std;

MockNoteTagListModel::MockNoteTagListModel()
	: isDirty (false)
{
}

void MockNoteTagListModel::ClearTag(const wstring & tag)
{
	clearedTag = tag;
}

void MockNoteTagListModel::Commit()
{
	SignalCommitted();
}

const TagList & MockNoteTagListModel::GetClearedTags()
{
	return cleared;
}

const TagList & MockNoteTagListModel::GetSelectedTags()
{
	return selected;
}

bool MockNoteTagListModel::IsDirty()
{
	return isDirty;
}

void MockNoteTagListModel::SelectTag(const wstring & tag)
{
	selectedTag = tag;
}

void MockNoteTagListModel::SetTags
	( const TagList & cleared
	, const TagList & selected
	)
{
	this->cleared  = cleared;
	this->selected = selected;
}