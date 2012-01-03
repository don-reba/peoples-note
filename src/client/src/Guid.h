#pragma once

#include <iterator>

class Guid
{
private:

	static const wchar_t localMarker;

	std::string data;

public:

	Guid();
	explicit Guid(std::string  data);
	explicit Guid(std::wstring data);

	bool IsLocal() const;

	bool IsEmpty() const;

	operator const std::string & () const;

	operator std::wstring () const;

	bool operator == (const Guid & guid) const;

	bool operator != (const Guid & guid) const;

	bool operator < (const Guid & guid) const;

public:

	static Guid GetEmpty();

private:

	static void FilterGuid
		( const wchar_t * guid
		, std::string   & result
		);
};

typedef std::vector<Guid> GuidList;
