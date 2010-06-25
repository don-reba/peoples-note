#pragma once

#include <iterator>

class Guid
{
private:

	std::string data;

public:

	Guid();
	Guid(std::string  data);
	Guid(std::wstring data);

	operator const std::string & () const;

	bool operator == (const Guid & guid) const;

	bool operator != (const Guid & guid) const;

private:

	static void FilterGuid
		( const wchar_t * guid
		, std::string   & result
		);
};
