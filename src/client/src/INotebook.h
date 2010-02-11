#pragma once

#include "INote.h"

class INotebook
{
private:

	std::wstring name;

public:

	virtual std::wstring GetName() const = 0;

	virtual void SetName(std::wstring name) = 0;

	virtual std::vector<INote*> GetNotes() const = 0;
};
