#include "stdafx.h"
#include "TagProcessor.h"

#include "IEnService.h"
#include "IUserModel.h"

TagProcessor::TagProcessor
	( IEnService & enService
	, IUserModel & userModel
	)
	: enService (enService)
	, userModel (userModel)
{
}

void TagProcessor::Add(const Tag & remote)
{
}

void TagProcessor::Delete(const Tag & local)
{
}

void TagProcessor::Rename(const Tag & local)
{
}

void TagProcessor::Upload(const Tag & local)
{
}

void TagProcessor::Merge
	( const Tag & local
	, const Tag & remote
	)
{
}
