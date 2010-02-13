#pragma once

#include "INotebook.h"
#include "Note.h"

class Notebook : public INotebook
{
private:

	std::wstring name;

	mutable std::vector<Note> notes;

public:

	virtual std::wstring GetName() const;

	virtual std::vector<INote*> GetNotes() const;

	virtual std::vector<INote*> GetNotes(std::wstring search) const;

	void SetName(std::wstring name);
};
