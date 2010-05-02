#pragma once

#include "IResourceProcessor.h"
#include "Tag.h"

class IEnService;
class IUserModel;

class TagProcessor : public IResourceProcessor<Tag>
{
private:

	IEnService & enService;
	IUserModel & userModel;

public:

	TagProcessor
		( IEnService & enService
		, IUserModel & userModel
		);

	virtual void Add    (const Tag & remote);
	virtual void Delete (const Tag & local);
	virtual void Rename (const Tag & local);
	virtual void Upload (const Tag & local);
	virtual void Merge
		( const Tag & local
		, const Tag & remote
		);
};
