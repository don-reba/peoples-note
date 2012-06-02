#pragma once

#include "Blob.h"

class ISqlBlob
{
public:

	virtual ~ISqlBlob() {}

	virtual int GetSize() = 0;

	virtual void Read(Blob & data) = 0;

	virtual void Read(int offset, int size, Blob & data) = 0;
};
