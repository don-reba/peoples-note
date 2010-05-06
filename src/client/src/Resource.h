#pragma once

#include "Blob.h"
#include "Guid.h"

class Resource
{
public:

	std::string Hash;
	::Blob      Data;
	::Guid      Guid;
	::Guid      Note;
};

typedef std::vector<Resource> ResourceList;
