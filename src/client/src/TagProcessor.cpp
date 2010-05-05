#include "stdafx.h"
#include "TagProcessor.h"

#include "INoteStore.h"
#include "IUserModel.h"

#include <algorithm>
#include <sstream>

using namespace std;

TagProcessor::TagProcessor
	( INoteStore & noteStore
	, IUserModel & userModel
	)
	: noteStore (noteStore)
	, userModel (userModel)
{
}

void TagProcessor::Add(const Tag & remote)
{
	userModel.AddTag(remote);
}

void TagProcessor::Delete(const Tag & local)
{
	userModel.DeleteTag(local);
}

void TagProcessor::Rename(const Tag & local)
{
	const TagList & tags(userModel.GetTags());

	vector<wstring> names;
	names.reserve(tags.size());
	foreach (const Tag & tag, tags)
		names.push_back(tag.GetName());
	sort(names.begin(), names.end());

	int n(2);
	wstringstream name;
	do
	{
		name.str(wstring());
		name << local.GetName() << L'(' << n << L')';
		++n;
	}
	while (binary_search(names.begin(), names.end(), name.str()));

	userModel.AddTag(Tag(name.str()));
}

void TagProcessor::Upload(const Tag & local)
{
	noteStore.CreateTag(local);
}

void TagProcessor::Merge
	( const Tag & local
	, const Tag & remote
	)
{
	// keep local
}
