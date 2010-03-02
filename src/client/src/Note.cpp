#include "stdafx.h"
#include "Note.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

Guid Note::GetGuid() const
{
	return guid;
}

wstring Note::GetTitle() const
{
	return title;
}

const ptr_vector<ITag> & Note::GetTags() const
{
	return tags;
}

const ITimestamp & Note::GetCreateDate() const
{
	return createDate;
}
