#pragma once

#include "IEnImporter.h"
#include "Note.h"

class MockEnImporter : public IEnImporter
{
public:

	std::vector<Note> notes;

public:

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		);
};
