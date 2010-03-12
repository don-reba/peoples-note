#pragma once

#include "IEnImporter.h"
#include "Note.h"

class MockEnImporter : public IEnImporter
{
public:

	NoteBodyList bodies;
	NoteList     notes;
	ImageList    images;

public:

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		, ImageList     & images
		);
};
