#pragma once

class INoteStore;
class IUserModel;
class Notebook;

class NotebookProcessor
{
private:

	IUserModel & userModel;

public:

	NotebookProcessor(IUserModel & userModel);

	void Add(const Notebook & remote);

	void Create
		( const Notebook & local
		, INoteStore     & noteStore
		);

	void Delete(const Notebook & local);

	void Merge
		( const Notebook & local
		, const Notebook & remote
		);

	void RenameAdd
		( const Notebook & local
		, const Notebook & remote
		);

	void Update
		( const Notebook & local
		, INoteStore     & noteStore
		);
};
