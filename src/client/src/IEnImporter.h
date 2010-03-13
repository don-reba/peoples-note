#pragma once

#include "Blob.h"
#include "Guid.h"
#include "Note.h"

#include <istream>

class IEnImporter
{
public:

	struct Image
	{
		Blob blob;
		Guid noteGuid;
	};
	typedef std::vector<Image> ImageList;

public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes
		( std::wistream & stream
		, NoteList      & notes
		, NoteBodyList  & bodies
		, ImageList     & images
		) = 0;
};
