#pragma once

#include "INote.h"

#include <istream>

class IEnImporter
{
public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes
		( std::wistream            & stream
		, boost::ptr_vector<INote> & notes
		) = 0;
};
