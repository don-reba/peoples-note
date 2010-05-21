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

	void Delete(const Notebook & local);

	void RenameAdd
		( const Notebook & local
		, const Notebook & remote
		);

	void Upload
		( const Notebook & local
		, INoteStore     & noteStore
		);

	void Merge
		( const Notebook & local
		, const Notebook & remote
		);
};
