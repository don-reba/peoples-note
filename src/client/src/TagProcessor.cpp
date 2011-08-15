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

void TagProcessor::AddLocal(const Tag & remote)
{
	userModel.AddTag(remote);
}

void TagProcessor::CreateRemote
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

void TagProcessor::DeleteLocal(const Tag & local)
{
	userModel.ExpungeTag(local.guid);
}

void TagProcessor::MergeLocal
		( const Tag & local
		, const Tag & remote
		)
{
	userModel.UpdateTag(local.guid, remote);
}

void TagProcessor::UpdateRemote
		( const Tag & local
		, INoteStore          & noteStore
		)
{
	Transaction transaction(userModel);

	Tag replacement;
	noteStore.UpdateTag(local,      replacement);
	userModel.UpdateTag(local.guid, replacement);
}
