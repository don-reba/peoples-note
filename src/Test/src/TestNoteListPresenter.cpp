#include "stdafx.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "NoteListPresenter.h"
#include "Note.h"
#include "Tag.h"
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

	TagList tags0;
	tags0.push_back(Tag(L"tag-0"));
	tags0.push_back(Tag(L"tag-1"));
	TagList tags1;
	TagList tags2;
	tags2.push_back(Tag(L"&amp;"));
	tags2.push_back(Tag(L"<strong>not bold</strong"));

	noteListModel.notes.push_back(Note(Guid(), L"Note",      Timestamp(0), tags0));
	noteListModel.notes.push_back(Note(Guid(), L"",          Timestamp(0), tags1));
	noteListModel.notes.push_back(Note(Guid(), L"<td id=\"", Timestamp(0), tags2));

	noteListModel.Reset();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes[0]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>Note</td></tr><tr><td>tag-0, tag-1</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[1]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td></td></tr><tr><td></td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[2]
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;, &lt;strong&gt;not bold&lt;/strong</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
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

	userModel.notes.push_back(Note(Guid(), L"note-0", Timestamp(0)));
	userModel.notes.push_back(Note(Guid(), L"note-1", Timestamp(0)));

	userModel.Loaded();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).GetTitle(), L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).GetTitle(), L"note-1");
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

	userModel.notebooks.push_back(Notebook(Guid(), L"notebook-0"));
	userModel.notebooks.push_back(Notebook(Guid(), L"notebook-1"));

	userModel.Loaded();

	BOOST_REQUIRE_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0), L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1), L"notebook-1");
}
