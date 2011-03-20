#pragma once 
#include "IFile.h"

class MockFile : public IFile
{
public:

	bool exists;
	Blob data;

public:

	virtual bool Read(const std::wstring & path, Blob & data);

	virtual bool Write(const std::wstring & path, const Blob & data);
};
