#pragma once

#include "Guid.h"

class INoteStore;
class IUserModel;
class Notebook;

class NotebookProcessor
{
private:

	IUserModel & userModel;

public:

	NotebookProcessor(IUserModel & userModel);

	void AddLocal(const Notebook & remote);

	void CreateRemote
		( const Notebook & local
		, INoteStore     & noteStore
		, Guid           & replacementGuid
		);

	void DeleteLocal(const Notebook & local);

	void MergeLocal
		( const Notebook & local
		, const Notebook & remote
		);

	void UpdateRemote
		( const Notebook & local
		, INoteStore     & noteStore
		, Guid           & replacementGuid
		);
};
