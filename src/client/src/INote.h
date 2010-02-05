#pragma once

#include "ITag.h"
#include "ITimestamp.h"

class INote
{
public:

	virtual std::wstring GetTitle() const = 0;

	virtual std::vector<const ITag*> GetTags() const = 0;

	virtual const ITimestamp & GetCreateDate() const = 0;
};
