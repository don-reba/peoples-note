#include "stdafx.h"
#include "Note.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(Note_Test)
{
	Note note;
	BOOST_CHECK_EQUAL(note.GetTitle(), L"");
	BOOST_CHECK_EQUAL(note.GetTags().size(), 0);
	BOOST_CHECK_EQUAL
		( note.GetCreateDate().GetFormattedDateTime()
		, L"1970-01-01 00:00"
		);
}
