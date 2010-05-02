#pragma once

#include "IResourceProcessor.h"
#include "Note.h"

class IEnService;
class IUserModel;
class Notebook;

class NoteProcessor : public IResourceProcessor<Note>
{
private:

	IEnService & enService;
	IUserModel & userModel;
	Notebook   & notebook;

public:

	NoteProcessor
		( IEnService & enService
		, IUserModel & userModel
		, Notebook   & notebook
		);

	virtual void Add    (const Note & remote);
	virtual void Delete (const Note & local);
	virtual void Rename (const Note & local);
	virtual void Upload (const Note & local);
	virtual void Merge
		( const Note & local
		, const Note & remote
		);
};
