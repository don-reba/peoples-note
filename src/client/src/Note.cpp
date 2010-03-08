#include "stdafx.h"
#include "Note.h"
#include "Tools.h"

#include <algorithm>

using namespace std;

Note::Note
	( Guid      guid
	, wstring   title
	, Timestamp creationDate
	)
	: guid         (guid)
	, title        (title)
	, creationDate (creationDate)
{
}

Note::Note
	( Guid            guid
	, std::wstring    title
	, Timestamp       creationDate
	, const TagList & tags
	)
	: guid         (guid)
	, title        (title)
	, creationDate (creationDate)
	, tags         (tags)
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

const TagList & Note::GetTags() const
{
	return tags;
}

const Timestamp & Note::GetCreationDate() const
{
	return creationDate;
}
