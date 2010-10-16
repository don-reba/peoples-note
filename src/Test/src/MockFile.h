#pragma once 
#include "IFile.h"

class MockFile : public IFile
{
public:

	bool exists;
	Blob data;

public:

	virtual bool Read(const std::wstring & path, Blob & data);
};
