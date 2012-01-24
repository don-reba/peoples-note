#pragma once
#include "IHtmlDataLoader.h"

class MockHtmlDataLoader : public IHtmlDataLoader
{
public:

	virtual UriType LoadFromUri(const wchar_t * uri, Blob & blob);
};
