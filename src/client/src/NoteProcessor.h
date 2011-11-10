#pragma once

#include "Guid.h"

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

	void AddLocal(const EnInteropNote & remote);

	void CreateRemote(const EnInteropNote & local);

	void DeleteLocal(const Guid & local);

	void DeleteRemote(const Guid & remote);

	void MergeLocal
		( const EnInteropNote & local
		, const EnInteropNote & remote
		);

	void UpdateRemote(const EnInteropNote & local);
};
