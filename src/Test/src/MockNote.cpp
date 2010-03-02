#include "stdafx.h"
#include "MockNote.h"

using namespace boost;
using namespace std;

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
