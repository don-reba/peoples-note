#pragma once

#include "INotebook.h"

class MockNotebook : public INotebook
{
public:

	std::wstring name;

public:

	MockNotebook();

	MockNotebook(const INotebook & notebook);

	virtual std::wstring GetName() const;
};
