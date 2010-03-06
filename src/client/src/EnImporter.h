#pragma once

#include "IEnImporter.h"

class EnImporter : public IEnImporter
{
public:

	virtual void ImportNotes
		( std::wistream            & stream
		, boost::ptr_vector<INote> & notes
		);
};
