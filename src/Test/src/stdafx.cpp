// stdafx.cpp : source file that includes just the standard includes
// Test.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include "Guid.h"
#include "Tools.h"

namespace std
{
	ostream & operator << (ostream & stream, const wchar_t * str)
	{
		return stream << Tools::ConvertToAnsi(str);
	}

	ostream & operator << (ostream & stream, const wstring & str)
	{
		return stream << Tools::ConvertToAnsi(str);
	}

	ostream & operator << (ostream & stream, const Guid & guid)
	{
		return stream << static_cast<const string &>(guid);
	}
}
