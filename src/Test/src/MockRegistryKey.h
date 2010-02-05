#pragma once

#include "IRegistryKey.h"

#include <map>

class MockRegistryKey : public IRegistryKey
{
public:

	typedef std::map<std::wstring, std::wstring> DataType;

	DataType data;

	virtual std::wstring GetString
		( std::wstring value
		, std::wstring defaultData
		) const;
};
