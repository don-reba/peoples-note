#include "stdafx.h"
#include "NoteListModel.h"

using namespace boost;
using namespace std;

class ExceptionHasText
{
private:

	const char * const text;

public:

	ExceptionHasText(const char * text)
		: text (text)
	{
	}

	bool operator () (const std::exception & e)
	{
		return strcmp(e.what(), text) == 0;
	}
};

BOOST_AUTO_TEST_CASE(NoteListModel_Empty_Test)
{
	NoteListModel noteListModel(2);

	NoteList::const_iterator begin;
	NoteList::const_iterator end;

	noteListModel.GetCurrentPage(begin, end);

	BOOST_CHECK(begin == end);

	BOOST_CHECK(!noteListModel.HasPreviousNotes());
	BOOST_CHECK(!noteListModel.HasNextNotes());

	BOOST_CHECK_EXCEPTION
		( noteListModel.SelectPreviousPage()
		, std::exception
		, ExceptionHasText("Invalid call to NoteListModel::SelectPreviousPage.")
		);
	BOOST_CHECK_EXCEPTION
		( noteListModel.SelectNextPage()
		, std::exception
		, ExceptionHasText("Invalid call to NoteListModel::SelectNextPage.")
		);
}

BOOST_AUTO_TEST_CASE(NoteListModel_Even_Test)
{
	NoteList::const_iterator begin;
	NoteList::const_iterator end;

	NoteListModel noteListModel(3);

	noteListModel.SetNotes(NoteList(6));

	BOOST_CHECK(!noteListModel.HasPreviousNotes());
	BOOST_CHECK(noteListModel.HasNextNotes());

	noteListModel.GetCurrentPage(begin, end);
	BOOST_CHECK(begin + 3 == end);

	noteListModel.SelectNextPage();

	BOOST_CHECK(noteListModel.HasPreviousNotes());
	BOOST_CHECK(!noteListModel.HasNextNotes());

	noteListModel.GetCurrentPage(begin, end);
	BOOST_CHECK(begin + 3 == end);
}

BOOST_AUTO_TEST_CASE(NoteListModel_Odd_Test)
{
	NoteList::const_iterator begin;
	NoteList::const_iterator end;

	NoteListModel noteListModel(3);

	noteListModel.SetNotes(NoteList(4));

	BOOST_CHECK(!noteListModel.HasPreviousNotes());
	BOOST_CHECK(noteListModel.HasNextNotes());

	noteListModel.GetCurrentPage(begin, end);
	BOOST_CHECK(begin + 3 == end);

	noteListModel.SelectNextPage();

	BOOST_CHECK(noteListModel.HasPreviousNotes());
	BOOST_CHECK(!noteListModel.HasNextNotes());

	noteListModel.GetCurrentPage(begin, end);
	BOOST_CHECK(begin + 1 == end);
}
