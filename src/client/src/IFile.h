#pragma once

#include "Blob.h"

class IFile
{
public:

	virtual bool Read(const std::wstring & path, Blob & data) = 0;
};
