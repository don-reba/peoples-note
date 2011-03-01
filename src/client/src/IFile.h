#pragma once

#include "Blob.h"

class IFile
{
public:

	virtual bool Read(const std::wstring & path, Blob & data) = 0;

	virtual bool Write(const std::wstring & path, const Blob & data) = 0;
};
