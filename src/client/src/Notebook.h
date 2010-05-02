#pragma once

#include "Guid.h"

class Notebook
{
private:

	Guid         guid;
	std::wstring name;
	int          usn;
	bool         isDirty;

public:

	Notebook(Guid guid, std::wstring name);

	std::wstring GetName() const;

	Guid GetGuid() const;

	int GetUsn() const;

	bool IsDirty() const;
};

typedef std::vector<Notebook> NotebookList;
