#pragma once

#include "Blob.h"

class IHtmlDataLoader
{
public:

	virtual bool LoadFromUri(const wchar_t * uri, Blob & blob) = 0;
};
