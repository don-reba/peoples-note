#pragma once

#include "IEnImporter.h"
#include "Note.h"

class MockEnImporter : public IEnImporter
{
public:

	NoteBodyList bodies;
	NoteList     notes;
	ResourceList resources;

public:

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		, ResourceList  & resources
		);
};
