#pragma once

class ITag;
class ITimestamp;

class INote
{
public:

	virtual ~INote() {}

	virtual std::wstring GetTitle() const = 0;

	virtual std::vector<const ITag*> GetTags() const = 0;

	virtual const ITimestamp & GetCreateDate() const = 0;
};
