#include "stdafx.h"
#include "MockNote.h"

using namespace boost;
using namespace std;

MockNote::MockNote()
{
}

MockNote::MockNote
	( Guid          guid
	, std::wstring  title
	, MockTimestamp createDate
	)
	: guid       (guid)
	, title      (title)
	, createDate (createDate)
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

const ptr_vector<ITag> & MockNote::GetTags() const
{
	return tags;
}

const ITimestamp & MockNote::GetCreateDate() const
{
	return createDate;
}
