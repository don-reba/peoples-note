#include "stdafx.h"
#include "TagProcessor.h"

#include "INoteStore.h"
#include "IUserModel.h"
#include "Tag.h"
#include "Transaction.h"

#include <algorithm>
#include <sstream>

using namespace std;

TagProcessor::TagProcessor(IUserModel & userModel)
	: userModel (userModel)
{
}

void TagProcessor::Add(const Tag & remote)
{
	userModel.AddTag(remote);
}

void TagProcessor::Create
		( const Tag & local
		, INoteStore          & noteStore
		)
{
	Transaction transaction(userModel);

	Tag replacement;
	noteStore.CreateTag(local, replacement);
	userModel.DeleteTag(local.guid);
	userModel.AddTag(replacement);
}

void TagProcessor::Delete(const Tag & local)
{
	userModel.DeleteTag(local.guid);
}

void TagProcessor::Merge
		( const Tag & local
		, const Tag & remote
		)
{
	userModel.UpdateTag(local, remote);
}

void TagProcessor::RenameAdd
		( const Tag & local
		, const Tag & remote
		)
{
	TagList tags;
	userModel.GetTags(tags);

	vector<wstring> names;
	names.reserve(tags.size());
	foreach (const Tag & tag, tags)
		names.push_back(tag.name);
	sort(names.begin(), names.end());

	int n(2);
	wstringstream name;
	do
	{
		name.str(wstring());
		name << local.name << L'(' << n << L')';
		++n;
	}
	while (binary_search(names.begin(), names.end(), name.str()));

	Tag tag(local);
	tag.name    = name.str();
	tag.isDirty = false;
	userModel.UpdateTag(local, tag);
	userModel.AddTag(remote);
}

void TagProcessor::Update
		( const Tag & local
		, INoteStore          & noteStore
		)
{
	Transaction transaction(userModel);

	Tag replacement;
	noteStore.UpdateTag(local, replacement);
	userModel.UpdateTag(local, replacement);
}
