#pragma once

#include "Guid.h"
#include "Note.h"

class EnInteropNote
{
private:

	Note note;
	std::vector<Guid> resources;

public:

	Guid GetGuid() const;

	std::wstring GetName() const;

	int GetUsn() const;

	bool IsDirty() const;

	const Note & GetNote() const;

	const std::vector<Guid> & GetResources() const;
};

typedef std::vector<EnInteropNote> EnInteropNoteList;
