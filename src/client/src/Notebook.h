#pragma once

#include "Guid.h"

class Notebook
{
private:

	Guid         guid;
	std::wstring name;

public:

	Notebook(Guid guid, std::wstring name);

	std::wstring GetName() const;

	Guid GetGuid() const;
};

typedef std::vector<Notebook> NotebookList;
