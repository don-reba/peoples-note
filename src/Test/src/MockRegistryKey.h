#pragma once

#include "IRegistryKey.h"

#include <map>

class MockRegistryKey : public IRegistryKey
{
public:

	typedef std::map<std::wstring, std::wstring> DataType;

	DataType data;

	virtual std::wstring GetString
		( const std::wstring & value
		, const std::wstring & defaultData
		) const;

	virtual void SetString
		( const std::wstring & value
		, const std::wstring & data
		);
};
