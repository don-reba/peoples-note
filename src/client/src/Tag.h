#pragma once

#include "Guid.h"

class Tag
{
public:

	std::wstring name;
	Guid         guid;
	int          usn;
	bool         isDirty;
};

typedef std::vector<Tag> TagList;
