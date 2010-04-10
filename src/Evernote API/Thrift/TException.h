#pragma once

#include "Protocol/TSTring.h"

#include <exception>
#include <string>

namespace Thrift
{

class TException : public std::exception
{
private:

	std::wstring message;
	std::string  ansi;

public:

	TException();

	TException(const wchar_t * message);

	virtual ~TException() throw();

	const wchar_t * GetMessage() const;

	virtual const char * what() const throw();


private:

	static std::string UnicodeToAnsi(const wchar_t * str);
};

} // Thrift
