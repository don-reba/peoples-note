#pragma once

#include "Guid.h"
#include "Note.h"
#include "RecognitionEntry.h"
#include "Resource.h"

#include <istream>

class IEnImporter
{
public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes
		( const std::wstring   & text
		, NoteList             & notes
		, NoteBodyList         & bodies
		, ResourceList         & resources
		, RecognitionEntryList & recognitionEntries
		) = 0;
};
