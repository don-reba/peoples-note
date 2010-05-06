#pragma once

#include "IEnImporter.h"

class EnImporter : public IEnImporter
{
public:

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		, ResourceList  & resources
		);

private:

	time_t ParseTime(std::wstring time);

	int GetDigit(wchar_t c);
};
