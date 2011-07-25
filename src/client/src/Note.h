#pragma once

#include "Guid.h"
#include "Location.h"
#include "Timestamp.h"

class Note
{
public:

	Guid         guid;
	Timestamp    creationDate;
	Timestamp    modificationDate;
	Timestamp    subjectDate;
	std::wstring name;
	int          usn;
	bool         isDirty;
	Location     Location;
	std::wstring Author;
	std::wstring Source;
	std::wstring SourceUrl;
	std::wstring SourceApplication;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
