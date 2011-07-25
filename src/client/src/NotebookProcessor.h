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

	void AddLocal(const Notebook & remote);

	void CreateRemote
		( const Notebook & local
		, INoteStore     & noteStore
		);

	void DeleteLocal(const Notebook & local);

	void MergeLocal
		( const Notebook & local
		, const Notebook & remote
		);

	void UpdateRemote
		( const Notebook & local
		, INoteStore     & noteStore
		);
};
