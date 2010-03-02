#include "stdafx.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "NoteListPresenter.h"
#include "MockNote.h"
#include "MockTag.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(NoteListPresenter_NoteListChanged_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

	ptr_vector<MockNote> notes(3);
	notes.push_back(new MockNote());
	notes.at(0).title = L"Note";
	notes.at(0).tags.push_back(new MockTag(L"tag-0"));
	notes.at(0).tags.push_back(new MockTag(L"tag-1"));
	notes.at(0).createDate.formattedDateTime = L"2010-02-04 15:20";
	notes.push_back(new MockNote());
	notes.at(1).title = L"";
	notes.at(1).createDate.formattedDateTime = L"";
	notes.push_back(new MockNote());
	notes.at(2).title = L"<td id=\"";
	notes.at(2).tags.push_back(new MockTag(L"&amp;"));
	notes.at(2).createDate.formattedDateTime = L"<strong>not bold</strong>";
	noteListModel.notes.transfer(noteListModel.notes.end(), notes);

	noteListModel.Reset();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes[0]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>Note</td></tr><tr><td>tag-1, tag-2</td></tr><tr><td>2010-02-04 15:20</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[1]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td></td></tr><tr><td></td></tr><tr><td></td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[2]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;</td></tr><tr><td>&lt;strong&gt;not bold&lt;/strong&gt;</td></tr></table>"
		);
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_LoadLastUsedNotebook_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

	userModel.lastUsedNotebook.name = L"notebook";

	ptr_vector<MockNote> notes(2);
	notes.push_back(new MockNote());
	notes.at(0).title = L"note-0";
	notes.push_back(new MockNote());
	notes.at(1).title = L"note-1";
	userModel.notes.transfer(userModel.notes.end(), notes);

	userModel.Loaded();

	BOOST_CHECK_EQUAL(userModel.notebookSelection, L"notebook");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes[0].GetTitle(), L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes[1].GetTitle(), L"note-1");
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_UpdateNotebookList_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

	ptr_vector<MockNotebook> notebooks(2);
	notebooks.push_back(new MockNotebook());
	notebooks[0].name = L"notebook-0";
	notebooks.push_back(new MockNotebook());
	notebooks[1].name = L"notebook-1";
	userModel.notebooks.transfer(userModel.notebooks.end(), notebooks);

	userModel.Loaded();

	BOOST_REQUIRE_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks[0], L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks[1], L"notebook-1");
}
