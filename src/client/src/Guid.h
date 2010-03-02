#pragma once

class Guid
{
private:

	std::string data;

public:

	Guid();
	Guid(std::string  data);
	Guid(std::wstring data);

	operator const std::string & ();
};