#pragma once

#include "Guid.h"

class Tag
{
private:

	std::wstring name;
	Guid         guid;
	int          usn;
	bool         isDirty;

public:

	Tag::Tag(std::wstring name);

	std::wstring GetName() const;

	Guid GetGuid() const;

	int GetUsn() const;

	bool IsDirty() const;
};

typedef std::vector<Tag> TagList;
