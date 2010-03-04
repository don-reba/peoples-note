#include "stdafx.h"
#include "MockNote.h"

#include <algorithm>

using namespace std;

MockNote::MockNote()
{
}

MockNote::MockNote
	( Guid          guid
	, std::wstring  title
	, MockTimestamp creationDate
	)
	: guid       (guid)
	, title      (title)
	, creationDate (creationDate)
{
}

Guid MockNote::GetGuid() const
{
	return guid;
}

wstring MockNote::GetTitle() const
{
	return title;
}

vector<const ITag*> MockNote::GetTags() const
{
	vector<const ITag*> tags;
	tags.reserve(this->tags.size());
	foreach (const ITag & tag, this->tags)
		tags.push_back(&tag);
	return tags;
}

const ITimestamp & MockNote::GetCreationDate() const
{
	return creationDate;
}
