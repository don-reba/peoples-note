#pragma once 
#include "IFile.h"

class File : public IFile
{
public:

	virtual bool Read(const std::wstring & path, Blob & data);

	virtual bool Write(const std::wstring & path, const Blob & data);
};
