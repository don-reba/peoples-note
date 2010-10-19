#pragma once

#include "IRegistryKey.h"

class RegistryKey : public IRegistryKey
{
private:

	HKEY key;

public:

	RegistryKey(const std::wstring & path);

	~RegistryKey();

	virtual std::wstring GetString
		( const wchar_t * value
		, const wchar_t * defaultData
		) const;

	virtual void SetString
		( const wchar_t * value
		, const wchar_t * data
		);
};
