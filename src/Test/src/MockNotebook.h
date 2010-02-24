#pragma once

#include "INotebook.h"

class MockNotebook : public INotebook
{
public:

	std::wstring name;

public:

	virtual std::wstring GetName() const;
};
