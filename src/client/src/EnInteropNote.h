#pragma once

#include "Guid.h"
#include "Note.h"

class EnInteropNote
{
public:

	Note              note;
	std::vector<Guid> resources;
	Guid              notebook;

	std::wstring name;
	Guid         guid;
	int          usn;
	bool         isDirty;
};

typedef std::vector<EnInteropNote> EnInteropNoteList;
