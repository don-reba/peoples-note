#pragma once

#include "INote.h"

class INotebook
{
public:

	virtual std::vector<INote*> GetNotes() = 0;
};
