#pragma once

#include "Guid.h"
#include "Timestamp.h"

class Note
{
public:

	Guid         guid;
	Timestamp    creationDate;
	std::wstring name;
	int          usn;
	bool         isDirty;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
