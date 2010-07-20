#pragma once

class IHtmlDataLoader
{
public:

	virtual BYTE * GetData() = 0;

	virtual DWORD GetDataSize() = 0;

	virtual bool LoadFromUri(const wchar_t * uri) = 0;
};
