#pragma once

#include <exception>

class ImagingException : public std::exception
{
private:

	DWORD   error;
	HRESULT result;

	std::string message;

public:

	ImagingException(HRESULT result);

	virtual const char * what() const;
};
