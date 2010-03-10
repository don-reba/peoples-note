#pragma once

#include "Guid.h"
#include "Tag.h"
#include "Timestamp.h"

class Note
{
private:

	Guid         guid;
	Timestamp    creationDate;
	std::wstring title;
	TagList      tags;

public:

	Note
		( Guid         guid
		, std::wstring title
		, Timestamp    creationDate
		);

	Note
		( Guid            guid
		, std::wstring    title
		, Timestamp       creationDate
		, const TagList & tags
		);

	Guid GetGuid() const;

	std::wstring GetTitle() const;

	const TagList & GetTags() const;

	const Timestamp & GetCreationDate() const;
};

typedef std::vector<std::wstring> NoteBodyList;
typedef std::vector<Note>         NoteList;
