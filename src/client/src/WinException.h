#pragma once 
#include <exception>

class WinException : public std::exception
{
private:

	std::string message;

public:

	WinException(DWORD error, const wchar_t * source);

	virtual const char * what() const;
};
