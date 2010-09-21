#pragma once

class EnInteropNote;
class EnNoteTranslator;
class INoteStore;
class IUserModel;
class Notebook;

class NoteProcessor
{
private:

	EnNoteTranslator & enNoteTranslator;
	INoteStore       & noteStore;
	IUserModel       & userModel;
	
	const Notebook & notebook;

public:

	NoteProcessor
		( EnNoteTranslator & enNoteTranslator
		, IUserModel       & userModel
		, INoteStore       & noteStore
		, const Notebook   & notebook
		);

	void Add(const EnInteropNote & remote);

	void Create(const EnInteropNote & local);

	void Delete(const EnInteropNote & local);

	void Merge
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);

	void RenameAdd
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);

	void Update(const EnInteropNote & local);
};
