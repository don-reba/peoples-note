#pragma once

#include "IRegistryKey.h"

#include <map>

class MockRegistryKey : public IRegistryKey
{
public:

	typedef std::map<std::wstring, std::wstring> DataType;

	DataType data;

public:

	virtual std::wstring GetString
		( const wchar_t * value
		, const wchar_t * defaultData
		) const;

	virtual void SetString
		( const wchar_t * value
		, const wchar_t * data
		);
};
