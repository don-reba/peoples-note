#pragma once

#include "Guid.h"
#include "Note.h"
#include "Resource.h"

#include <istream>

class IEnImporter
{
public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		, ResourceList  & images
		) = 0;
};
