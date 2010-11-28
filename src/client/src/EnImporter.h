#pragma once

#include "IEnImporter.h"

class EnImporter : public IEnImporter
{
public:

	virtual void ImportNotes
		( const std::wstring   & text
		, NoteList             & notes
		, NoteBodyList         & bodies
		, ResourceList         & resources
		, RecognitionEntryList & recognitionEntries
		);

private:

	time_t ParseTime(std::wstring time);

	int GetDigit(wchar_t c);
};
