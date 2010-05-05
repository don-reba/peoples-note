#include "stdafx.h"
#include "Note.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "SearchPresenter.h"

using namespace boost;
using namespace std;

Note MakeNote(const wchar_t * name)
{
	Note note;
	note.name = name;
	return note;
}

BOOST_AUTO_TEST_CASE(SearchPresenter_Search_Test1)
{
	MockNoteListModel noteListModel;
	MockUserModel     userModel;
	MockNoteListView  noteListView;
	SearchPresenter   searchPresenter(noteListModel, userModel, noteListView);

	userModel.notes.push_back(MakeNote(L"note-0"));
	userModel.notes.push_back(MakeNote(L"note-1"));

	noteListView.searchString = L"search";
	noteListView.SignalSearch();

	BOOST_CHECK_EQUAL(userModel.searchSelection, L"search");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");
}

BOOST_AUTO_TEST_CASE(SearchPresenter_SearchReset_Test1)
{
	MockNoteListModel noteListModel;
	MockUserModel     userModel;
	MockNoteListView  noteListView;
	SearchPresenter   searchPresenter(noteListModel, userModel, noteListView);

	userModel.notes.push_back(MakeNote(L"note-0"));
	userModel.notes.push_back(MakeNote(L"note-1"));

	noteListView.searchString = L"";
	noteListView.SignalSearch();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");
}
