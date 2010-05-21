#pragma once

class EnInteropNote;
class INoteStore;
class IUserModel;
class Notebook;

class NoteProcessor
{
private:

	IUserModel & userModel;

public:

	NoteProcessor(IUserModel & userModel);

	void Add
		( const EnInteropNote & remote
		, INoteStore          & noteStore
		, Notebook            & notebook
		);

	void Delete(const EnInteropNote & local);

	void RenameAdd
		( const EnInteropNote & local
		, const EnInteropNote & remote
		, INoteStore          & noteStore
		, Notebook            & notebook
		);

	void Upload
		( const EnInteropNote & local
		, INoteStore          & noteStore
		, Notebook            & notebook
		);

	void Merge
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);
};
