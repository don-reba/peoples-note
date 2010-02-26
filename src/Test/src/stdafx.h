// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>

#define BOOST_TEST_MODULE general test
#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "foreach.h"

namespace std
{
	ostream & operator << (ostream & stream, const wchar_t * str);
	ostream & operator << (ostream & stream, const wstring & str);
}
