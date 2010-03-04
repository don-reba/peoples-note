#include "stdafx.h"
#include "Note.h"
#include "Tools.h"

#include <algorithm>

using namespace std;

Note::Note(Guid guid, wstring title)
	: guid  (guid)
	, title (title)
{
}

Guid Note::GetGuid() const
{
	return guid;
}

wstring Note::GetTitle() const
{
	return title;
}

std::vector<const ITag*> Note::GetTags() const
{
	vector<const ITag*> tags;
	tags.reserve(this->tags.size());
	foreach (const ITag & tag, this->tags)
		tags.push_back(&tag);
	return tags;
}

const ITimestamp & Note::GetCreationDate() const
{
	return creationDate;
}
