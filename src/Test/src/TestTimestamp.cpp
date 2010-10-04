#include "stdafx.h"
#include "Timestamp.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test1)
{
	Timestamp timestamp(0);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"1970-01-01 02:00"
		);
}

BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test2)
{
	Timestamp timestamp(0x3DE43B0C);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"2002-11-27 05:25"
		);
}
