#include "stdafx.h"
#include "NoteListModel.h"

#include "MockRegistryKey.h"
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
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

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
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel, registryKey);

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
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel, registryKey);

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
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

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

BOOST_AUTO_TEST_CASE(NoteListModel_NotebookTitle)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

	noteListModel.SetNotebookTitleState(true);
	BOOST_CHECK_EQUAL(registryKey.GetString(L"notebook title state", L""), L"enabled");
	BOOST_CHECK(noteListModel.GetNotebookTitleState());

	noteListModel.SetNotebookTitleState(false);
	BOOST_CHECK_EQUAL(registryKey.GetString(L"notebook title state", L""), L"disabled");
	BOOST_CHECK(!noteListModel.GetNotebookTitleState());
}
