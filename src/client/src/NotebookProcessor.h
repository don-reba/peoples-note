#pragma once

#include "IResourceProcessor.h"
#include "Notebook.h"

class IEnService;
class IUserModel;

class NotebookProcessor : public IResourceProcessor<Notebook>
{
private:

	IEnService & enService;
	IUserModel & userModel;

public:

	NotebookProcessor
		( IEnService & enService
		, IUserModel & userModel
		);

	virtual void Add    (const Notebook & remote);
	virtual void Delete (const Notebook & local);
	virtual void Rename (const Notebook & local);
	virtual void Upload (const Notebook & local);
	virtual void Merge
		( const Notebook & local
		, const Notebook & remote
		);
};
