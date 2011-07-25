#pragma once

#include "Guid.h"
#include "Timestamp.h"

class Notebook
{
public:

	Guid         guid;
	std::wstring name;
	Timestamp    CreationDate;
	Timestamp    ModificationDate;
	int          usn;
	bool         isDirty;
};

typedef std::vector<Notebook> NotebookList;
