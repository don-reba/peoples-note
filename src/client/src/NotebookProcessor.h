#pragma once

#include "IResourceProcessor.h"
#include "Notebook.h"

class INoteStore;
class IUserModel;

class NotebookProcessor : public IResourceProcessor<Notebook>
{
private:

	INoteStore & noteStore;
	IUserModel & userModel;

public:

	NotebookProcessor
		( INoteStore & noteStore
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
