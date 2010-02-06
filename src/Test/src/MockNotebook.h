#pragma once

#include "INotebook.h"

class MockNotebook : public INotebook
{
public:

	std::vector<INote*> notes;

	virtual std::vector<INote*> GetNotes();
};
