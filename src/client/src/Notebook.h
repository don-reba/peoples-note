#pragma once

#include "INotebook.h"

class Notebook : public INotebook
{
private:

	Guid         guid;
	std::wstring name;

public:

	Notebook(Guid guid, std::wstring name);

	virtual std::wstring GetName() const;

	virtual Guid GetGuid() const;
};
