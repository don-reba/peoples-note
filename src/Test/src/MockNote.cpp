#include "stdafx.h"
#include "MockNote.h"

using namespace std;

wstring MockNote::GetTitle() const
{
	return title;
}

std::vector<const ITag*> MockNote::GetTags() const
{
	std::vector<const ITag*> tags;
	tags.reserve(this->tags.size());
	foreach (const MockTag & tag, this->tags)
		tags.push_back(&tag);
	return tags;
}

const ITimestamp & MockNote::GetCreateDate() const
{
	return createDate;
}
