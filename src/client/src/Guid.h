#pragma once

#include <iterator>

class Guid
{
private:

	static const wchar_t localMarker;

	std::string data;

public:

	Guid();
	Guid(std::string  data);
	Guid(std::wstring data);

	bool IsLocal() const;

	operator const std::string & () const;

	bool operator == (const Guid & guid) const;

	bool operator != (const Guid & guid) const;

private:

	static void FilterGuid
		( const wchar_t * guid
		, std::string   & result
		);
};
