#pragma once

#include "Guid.h"
#include "Note.h"

class EnInteropNote
{
public:

	Note note;
	std::vector<Guid> resources;

public:

	Guid GetGuid() const;

	std::wstring GetName() const;

	int GetUsn() const;

	bool IsDirty() const;
};

typedef std::vector<EnInteropNote> EnInteropNoteList;
