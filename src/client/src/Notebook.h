#pragma once

#include "INotebook.h"

class Notebook : public INotebook
{
private:

	std::wstring name;

public:

	Notebook(std::wstring name);

	virtual std::wstring GetName() const;
};
