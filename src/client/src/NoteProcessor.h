#pragma once

#include "Guid.h"

class EnInteropNote;
class EnNoteTranslator;
class INoteStore;
class IUserModel;

class NoteProcessor
{
private:

	EnNoteTranslator & enNoteTranslator;
	INoteStore       & noteStore;
	IUserModel       & userModel;

public:

	NoteProcessor
		( EnNoteTranslator & enNoteTranslator
		, IUserModel       & userModel
		, INoteStore       & noteStore
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
