#include "stdafx.h"
#include "Timestamp.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test1)
{
	Timestamp timestamp(0);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"1969-12-31 18:00"
		);
}

BOOST_AUTO_TEST_CASE(Timestamp_GetFormattedDateTime_Test2)
{
	Timestamp timestamp(0x3DE43B0C);
	BOOST_CHECK_EQUAL
		( timestamp.GetFormattedDateTime()
		, L"2002-11-26 21:25"
		);
}
