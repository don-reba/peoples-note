#include "stdafx.h"
#include "MockRegistryKey.h"

using namespace std;

wstring MockRegistryKey::GetString
		( const wstring & value
		, const wstring & defaultData
		) const
{
	DataType::const_iterator datum = data.find(value);
	if (data.end() == datum)
		return defaultData;
	else
		return datum->second;
}

void MockRegistryKey::SetString
	( const wstring & value
	, const wstring & data
	)
{
	this->data[value] = data;
}
