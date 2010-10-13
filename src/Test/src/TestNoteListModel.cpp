#include "stdafx.h"
#include "NoteListModel.h"

#include "MockUserModel.h"
#include "SignalCheck.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

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

//-----------
// test cases
//-----------

BOOST_AUTO_TEST_CASE(NoteListModel_Empty)
{
	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel);

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

BOOST_AUTO_TEST_CASE(NoteListModel_Even)
{
	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel);

	NoteList::const_iterator begin;
	NoteList::const_iterator end;

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

BOOST_AUTO_TEST_CASE(NoteListModel_Odd)
{
	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel);

	NoteList::const_iterator begin;
	NoteList::const_iterator end;

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

BOOST_AUTO_TEST_CASE(NoteListModel_Reload)
{
	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel);

	NoteList::const_iterator begin;
	NoteList::const_iterator end;

	userModel.notebookSelection = L"";
	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note 0";
	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note 1";

	NoteList notes(3);
	noteListModel.SetNotes(notes);
	noteListModel.SelectNextPage();

	noteListModel.GetCurrentPage(begin, end);
	BOOST_CHECK(begin + 1 == end);

	SignalCheck signalChangedCheck;
	noteListModel.ConnectChanged(ref(signalChangedCheck));

	noteListModel.Reload();

	BOOST_CHECK_EQUAL(userModel.notebookSelection, L"last-used-notebook");

	noteListModel.GetCurrentPage(begin, end);
	BOOST_REQUIRE(begin + 2 == end);
	BOOST_CHECK_EQUAL(begin[0].name, L"note 0");
	BOOST_CHECK_EQUAL(begin[1].name, L"note 1");

	BOOST_CHECK(signalChangedCheck);
}
