#pragma once

#include "Blob.h"
#include "IHtmlDataLoader.h"

class HtmlDataLoader : public IHtmlDataLoader
{
private:

	Blob blob;

public:

	virtual void * GetData();

	virtual DWORD GetDataSize();

	virtual LoadFromUri(const wchar_t * uri);
};
