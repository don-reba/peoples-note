#include "stdafx.h"
#include "Note.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(Note_Test)
{
	Guid    guid;
	wstring title(L"test");
	Note note(guid, title);
	BOOST_CHECK_EQUAL
		( static_cast<string>(note.GetGuid())
		, static_cast<string>(guid)
		);
	BOOST_CHECK_EQUAL(note.GetTitle(), L"test");
	BOOST_CHECK_EQUAL(note.GetTags().size(), 0);
	BOOST_CHECK_EQUAL
		( note.GetCreationDate().GetFormattedDateTime()
		, L"1970-01-01 00:00"
		);
}
