#include "stdafx.h"
#include "MockRegistryKey.h"

using namespace std;

wstring MockRegistryKey::GetString
	( const wchar_t * value
	, const wchar_t * defaultData
	) const
{
	DataType::const_iterator datum = data.find(value);
	if (data.end() == datum)
		return defaultData;
	else
		return datum->second;
}

void MockRegistryKey::SetString
	( const wchar_t * value
	, const wchar_t * data
	)
{
	this->data[value] = data;
}
