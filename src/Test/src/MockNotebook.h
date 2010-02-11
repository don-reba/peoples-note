#pragma once

#include "INotebook.h"

class MockNotebook : public INotebook
{
public:

	std::vector<INote*> notes;
	std::wstring        name;

public:

	virtual std::wstring GetName() const;

	virtual void SetName(std::wstring name);

	virtual std::vector<INote*> GetNotes() const;
};
