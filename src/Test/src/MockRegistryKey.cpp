#include "stdafx.h"
#include "MockRegistryKey.h"

using namespace std;

wstring MockRegistryKey::GetString
		( wstring value
		, wstring defaultData
		) const
{
	DataType::const_iterator datum = data.find(value);
	if (data.end() == datum)
		return defaultData;
	else
		return datum->second;
}
