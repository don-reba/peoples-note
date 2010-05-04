#pragma once

#include "IResourceProcessor.h"
#include "EnInteropNote.h"

class INoteStore;
class IUserModel;
class Notebook;

class NoteProcessor : public IResourceProcessor<EnInteropNote>
{
private:

	INoteStore & noteStore;
	IUserModel & userModel;
	Notebook   & notebook;

public:

	NoteProcessor
		( INoteStore & noteStore
		, IUserModel & userModel
		, Notebook   & notebook
		);

	virtual void Add    (const EnInteropNote & remote);
	virtual void Delete (const EnInteropNote & local);
	virtual void Rename (const EnInteropNote & local);
	virtual void Upload (const EnInteropNote & local);
	virtual void Merge
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);
};
