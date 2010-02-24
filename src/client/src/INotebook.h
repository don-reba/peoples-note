#pragma once

class INote;

class INotebook
{
public:

	virtual std::wstring GetName() const = 0;

	virtual std::vector<INote*> GetNotes() const = 0;

	virtual std::vector<INote*> GetNotes(std::wstring search) const = 0;
};
