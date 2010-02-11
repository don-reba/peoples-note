#include "stdafx.h"
#include "Note.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

wstring Note::GetTitle() const
{
	return title;
}

vector<const ITag*> Note::GetTags() const
{
	vector<const ITag*> tags;
	tags.reserve(this->tags.size());
	foreach (const shared_ptr<ITag> & tag, this->tags)
		tags.push_back(&*tag);
	return tags;
}

const ITimestamp & Note::GetCreateDate() const
{
	return createDate;
}
