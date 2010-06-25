#include "stdafx.h"
#include "Guid.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

Guid::Guid()
{
	GUID guid = { 0 };
	if (S_OK != ::CoCreateGuid(&guid))
		throw exception("GUID creation failed.");

	vector<wchar_t> str(40);
	if (0 == ::StringFromGUID2(guid, &str[0], str.size()))
		throw exception("GUID to string conversion failed.");

	data.clear();
	FilterGuid(&str[0], data);
}

Guid::Guid(string data)
	: data (data)
{
}

Guid::Guid(wstring data)
	: data (ConvertToAnsi(data))
{
}

Guid::operator const string & () const
{
	return data;
}

bool Guid::operator == (const Guid & guid) const
{
	return data == guid.data;
}

bool Guid::operator != (const Guid & guid) const
{
	return data != guid.data;
}

void Guid::FilterGuid
	( const wchar_t * guid
	, string        & result
	)
{
	result.clear();
	for (; *guid; ++guid)
	{
		wchar_t c(*guid);
		if
			(  (c >= L'A' && c <= L'F')
			|| (c >= L'a' && c <= L'f')
			|| (c >= L'0' && c <= L'9')
			)
		{
			// these Unicode characters can be cast safely
			result.push_back(static_cast<char>(c));
		}
	}
}
