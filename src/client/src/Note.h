#pragma once

#include "Guid.h"
#include "Tag.h"
#include "Timestamp.h"

class Note
{
public:

	Guid         guid;
	Timestamp    creationDate;
	std::wstring name;
	TagList      tags;
	int          usn;
	bool         isDirty;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
