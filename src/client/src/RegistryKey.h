#pragma once

#include "IRegistryKey.h"

class RegistryKey : public IRegistryKey
{
public:

	virtual std::wstring GetString
		( std::wstring value
		, std::wstring defaultData
		) const;
};
