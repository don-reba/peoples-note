#pragma once

#include "IEnImporter.h"
#include "MockNote.h"

class MockEnImporter : public IEnImporter
{
public:

	std::wstring path;
	std::vector<MockNote> notes;

public:

	virtual void ImportNotes(std::wstring path, boost::ptr_vector<INote> &notes);
};