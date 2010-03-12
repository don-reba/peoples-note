#pragma once

#include "Blob.h"

class ISqlBlob
{
public:

	virtual void Read(Blob & blob)= 0;
};
