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

public:

	Note();

	Note
		( Guid         guid
		, std::wstring name
		, Timestamp    creationDate
		, int          usn
		, bool         isDirty
		);

	Note
		( Guid            guid
		, std::wstring    name
		, Timestamp       creationDate
		, int             usn
		, bool            isDirty
		, const TagList & tags
		);

	Guid GetGuid() const;

	std::wstring GetName() const;

	const TagList & GetTags() const;

	const Timestamp & GetCreationDate() const;

	int GetUsn() const;

	bool IsDirty() const;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
