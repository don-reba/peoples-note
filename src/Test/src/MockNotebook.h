#pragma once

#include "INotebook.h"

class MockNotebook : public INotebook
{
public:

	Guid         guid;
	std::wstring name;

public:

	MockNotebook();

	MockNotebook(std::wstring name);

	MockNotebook(const INotebook & notebook);

	virtual std::wstring GetName() const;

	virtual Guid GetGuid() const;
};
