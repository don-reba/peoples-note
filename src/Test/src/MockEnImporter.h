#pragma once

#include "IEnImporter.h"
#include "Note.h"

class MockEnImporter : public IEnImporter
{
public:

	NoteBodyList         bodies;
	NoteList             notes;
	ResourceList         resources;
	RecognitionEntryList recognitionEntries;
	std::wstring         text;

public:

	virtual void ImportNotes
		( const std::wstring   & text
		, NoteList             & notes
		, NoteBodyList         & bodies
		, ResourceList         & resources
		, RecognitionEntryList & recognitionEntries
		);
};
