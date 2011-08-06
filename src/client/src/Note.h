#pragma once

#include "Guid.h"
#include "Location.h"
#include "Timestamp.h"

class Note
{
public:

	Guid         guid;
	int          usn;
	std::wstring name;
	bool         isDirty;
	Timestamp    creationDate;
	Timestamp    modificationDate;
	Timestamp    subjectDate;
	Location     Location;
	std::wstring Author;
	std::wstring Source;
	std::wstring SourceUrl;
	std::wstring SourceApplication;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
