#pragma once
#include "INoteTagListView.h"

class MockNoteTagListView : public INoteTagListView
{
	MacroTestEvent(Cancel)
	MacroTestEvent(Ok)
	MacroTestEvent(TagCleared)
	MacroTestEvent(TagSelected)

public:

	bool         isShown;
	TagList      clearedTags;
	TagList      selectedTags;
	std::wstring activeTag;

public:

	MockNoteTagListView();

public:

	virtual void Hide();

	virtual std::wstring GetActiveTag();

	virtual void SetTags
		( const TagList & cleared
		, const TagList & selected
		);

	virtual void Show();
};
