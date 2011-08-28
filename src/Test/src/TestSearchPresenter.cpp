#include "stdafx.h"
#include "Note.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "SearchPresenter.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct SearchPresenterFixture
{
	MockNoteListModel noteListModel;
	MockUserModel     userModel;
	MockNoteListView  noteListView;

	SearchPresenter searchPresenter;

	SearchPresenterFixture()
		: searchPresenter
			( noteListModel
			, userModel
			, noteListView
			)
	{
	}
};

Note MakeNote(const wchar_t * name)
{
	Note note;
	note.name = name;
	return note;
}

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( SearchPresenter_Clear_Test
	, SearchPresenterFixture
	)
{
	userModel.notes.push_back(MakeNote(L"note-0"));
	userModel.notes.push_back(MakeNote(L"note-1"));

	noteListView.isSearchButtonSetToSearch = false;

	noteListView.searchString = L"search";
	noteListView.SignalClearSearch();

	BOOST_CHECK(noteListModel.isReloaded);

	BOOST_CHECK_EQUAL(noteListView.searchText, L"");
	BOOST_CHECK(noteListView.isSearchButtonSetToSearch);
}

BOOST_FIXTURE_TEST_CASE
	( SearchPresenter_Search_Test
	, SearchPresenterFixture
	)
{
	userModel.notes.push_back(MakeNote(L"note-0"));
	userModel.notes.push_back(MakeNote(L"note-1"));

	noteListView.searchString = L"search";
	noteListView.SignalSearch();

	BOOST_CHECK_EQUAL(userModel.searchSelection, L"search");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");

	BOOST_CHECK(!noteListView.isSearchButtonSetToSearch);
}

BOOST_FIXTURE_TEST_CASE
	( SearchPresenter_SearchReset_Test
	, SearchPresenterFixture
	)
{
	userModel.notes.push_back(MakeNote(L"note-0"));
	userModel.notes.push_back(MakeNote(L"note-1"));

	noteListView.searchString = L"";
	noteListView.SignalSearch();

	BOOST_CHECK(noteListModel.isReloaded);

	BOOST_CHECK(noteListView.isSearchButtonSetToSearch);
}
