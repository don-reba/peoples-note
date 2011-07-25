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
	userModel.ExpungeTag(local.guid);
	userModel.AddTag(replacement);
}

void TagProcessor::Delete(const Tag & local)
{
	userModel.ExpungeTag(local.guid);
}

void TagProcessor::Merge
		( const Tag & local
		, const Tag & remote
		)
{
	userModel.UpdateTag(local, remote);
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
