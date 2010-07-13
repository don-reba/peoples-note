#pragma once

class IHtmlDataLoader
{
public:

	virtual void * GetData() = 0;

	virtual DWORD GetDataSize() = 0;

	virtual LoadFromUri(const wchar_t * uri) = 0;
};
