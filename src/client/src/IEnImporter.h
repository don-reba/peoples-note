#pragma once

#include "Note.h"

#include <istream>

class IEnImporter
{
public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		) = 0;
};
