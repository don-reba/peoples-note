#include "stdafx.h"
#include "Note.h"
#include "Tools.h"

#include <algorithm>

using namespace std;

Note::Note
	( Guid      guid
	, wstring   name
	, Timestamp creationDate
	, int       usn
	, bool      isDirty
	)
	: guid         (guid)
	, name         (name)
	, creationDate (creationDate)
	, usn          (usn)
	, isDirty      (isDirty)
{
}

Note::Note
	( Guid            guid
	, std::wstring    name
	, Timestamp       creationDate
	, int             usn
	, bool            isDirty
	, const TagList & tags
	)
	: guid         (guid)
	, name         (name)
	, creationDate (creationDate)
	, usn          (usn)
	, isDirty      (isDirty)
	, tags         (tags)
{
}

Guid Note::GetGuid() const
{
	return guid;
}

wstring Note::GetName() const
{
	return name;
}

const TagList & Note::GetTags() const
{
	return tags;
}

const Timestamp & Note::GetCreationDate() const
{
	return creationDate;
}

int Note::GetUsn() const
{
	return usn;
}

bool Note::IsDirty() const
{
	return isDirty;
}
