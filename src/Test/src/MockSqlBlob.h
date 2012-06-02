#pragma once
#include "ISqlBlob.h"

class MockSqlBlob : public ISqlBlob
{
public:

	Blob data;

public:

	virtual int GetSize();

	virtual void Read(Blob & data);

	virtual void Read(int offset, int size, Blob & data);
};