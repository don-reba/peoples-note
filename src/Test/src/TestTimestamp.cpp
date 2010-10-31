#include "stdafx.h"
#include "Timestamp.h"

using namespace boost;
using namespace std;

// Note: the output of this test depends on your time zone.
BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test1)
{
	Timestamp timestamp(0);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"1970-01-01 01:00"
		);
}

// Note: the output of this test depends on your time zone.
BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test2)
{
	Timestamp timestamp(0x3DE43B0C);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"2002-11-27 04:25"
		);
}
