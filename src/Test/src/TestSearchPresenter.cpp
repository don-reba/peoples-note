#include "stdafx.h"
#include "MockNote.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "SearchPresenter.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(SearchPresenter_Search_Test1)
{
	MockNoteListModel noteListModel;
	MockUserModel     userModel;
	MockNoteListView  noteListView;
	SearchPresenter   searchPresenter(noteListModel, userModel, noteListView);

	vector<MockNote> notes(2);
	notes.at(0).title = L"note-0";
	notes.at(1).title = L"note-1";
	foreach (MockNote & note, notes)
		userModel.notes.push_back(&note);

	noteListView.searchString = L"search";
	noteListView.SignalSearch();

	BOOST_CHECK_EQUAL(userModel.searchSelection, L"search");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0)->GetTitle(), L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1)->GetTitle(), L"note-1");
}

BOOST_AUTO_TEST_CASE(SearchPresenter_SearchReset_Test1)
{
	MockNoteListModel noteListModel;
	MockUserModel     userModel;
	MockNoteListView  noteListView;
	SearchPresenter   searchPresenter(noteListModel, userModel, noteListView);

	userModel.lastUsedNotebook.name = L"notebook";

	vector<MockNote> notes(2);
	notes.at(0).title = L"note-0";
	notes.at(1).title = L"note-1";
	foreach (MockNote & note, notes)
		userModel.notes.push_back(&note);

	noteListView.searchString = L"";
	noteListView.SignalSearch();

	BOOST_CHECK_EQUAL(userModel.notebookSelection, L"notebook");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0)->GetTitle(), L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1)->GetTitle(), L"note-1");
}
