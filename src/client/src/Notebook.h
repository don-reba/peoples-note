#pragma once

#include "INotebook.h"
#include "Note.h"

class Notebook : public INotebook
{
private:

	mutable std::vector<Note> notes;

public:

	Notebook();

	virtual std::wstring GetName() const;

	virtual std::vector<INote*> GetNotes() const;
};
