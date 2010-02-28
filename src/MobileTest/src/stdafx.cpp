// stdafx.cpp : source file that includes just the standard includes
// MobileTest.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "Tools.h"

namespace std
{
	wostream & operator << (wostream & stream, const char * str)
	{
		return stream << Tools::ConvertToUnicode(str);
	}

	wostream & operator << (wostream & stream, const string & str)
	{
		return stream << Tools::ConvertToUnicode(str);
	}
}
