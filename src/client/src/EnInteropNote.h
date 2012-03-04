#pragma once

#include "Guid.h"
#include "Note.h"

class EnInteropNote
{
public:

	Note     note;
	GuidList resources;
	GuidList tags;
	Guid     notebook;

	std::wstring name;
	Guid         guid;
	int          usn;
	bool         isDirty;

	EnInteropNote(const Note & note, const Guid & notebook);
};

typedef std::vector<EnInteropNote> EnInteropNoteList;
