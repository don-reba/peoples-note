#pragma once

#include "INote.h"

class IEnImporter
{
public:

	virtual ~IEnImporter() {}

	virtual void ImportNotes(std::wstring path, boost::ptr_vector<INote> &notes) = 0;
};
