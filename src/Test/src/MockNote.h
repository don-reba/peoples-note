#pragma once

#include "INote.h"
#include "MockTag.h"
#include "MockTimestamp.h"

class MockNote : public INote
{
public:

	std::wstring         title;
	std::vector<MockTag> tags;
	MockTimestamp        createDate;

public:

	virtual std::wstring GetTitle() const;

	virtual std::vector<const ITag*> GetTags() const;

	virtual const ITimestamp & GetCreateDate() const;
};
