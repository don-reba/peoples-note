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
		( const std::wstring & value
		, const std::wstring & defaultData
		) const;

	virtual void SetString
		( const std::wstring & value
		, const std::wstring & data
		);
};
