#pragma once

#include "Guid.h"

class Notebook
{
public:

	Guid         guid;
	std::wstring name;
	int          usn;
	bool         isDirty;
};

typedef std::vector<Notebook> NotebookList;
