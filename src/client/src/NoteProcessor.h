#pragma once

class EnInteropNote;
class INoteStore;
class IUserModel;
class Notebook;

class NoteProcessor
{
private:

	INoteStore & noteStore;
	IUserModel & userModel;
	
	const Notebook & notebook;

public:

	NoteProcessor
		( IUserModel     & userModel
		, INoteStore     & noteStore
		, const Notebook & notebook
		);

	void Add(const EnInteropNote & remote);

	void Delete(const EnInteropNote & local);

	void RenameAdd
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);

	void Upload(const EnInteropNote & local);

	void Merge
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);
};
