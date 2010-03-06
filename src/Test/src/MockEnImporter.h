#pragma once

#include "IEnImporter.h"
#include "MockNote.h"

class MockEnImporter : public IEnImporter
{
public:

	std::vector<MockNote> notes;

public:

	virtual void ImportNotes
		( std::wistream            & stream
		, boost::ptr_vector<INote> & notes
		);
};
