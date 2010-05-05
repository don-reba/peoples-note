#include "stdafx.h"
#include "EnNoteTranslator.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NoteListPresenter.h"
#include "Note.h"
#include "Tag.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

Note MakeNote(const Guid & guid, const wchar_t * name, int creationDate, const TagList & tags)
{
	Note note;
	note.guid         = guid;
	note.name         = name;
	note.creationDate = creationDate;
	copy(tags.begin(), tags.end(), back_inserter(note.tags));
	return note;
}

Note MakeNote(const Guid & guid, const wchar_t * name, int creationDate)
{
	Note note;
	note.guid         = guid;
	note.name         = name;
	note.creationDate = creationDate;
	return note;
}

Note MakeNote(const wchar_t * name, int creationDate)
{
	Note note;
	note.name         = name;
	note.creationDate = creationDate;
	return note;
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_NoteListChanged_Test)
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	TagList tags0;
	tags0.push_back(Tag());
	tags0.back().name = L"tag-0";
	tags0.push_back(Tag());
	tags0.back().name = L"tag-1";
	TagList tags1;
	TagList tags2;
	tags2.push_back(Tag());
	tags2.back().name = L"&amp;";
	tags2.push_back(Tag());
	tags2.back().name = L"<strong>not bold</strong";

	noteListModel.notes.push_back(MakeNote(Guid("{0}"), L"Note",      0, tags0));
	noteListModel.notes.push_back(MakeNote(Guid("{1}"), L"",          0, tags1));
	noteListModel.notes.push_back(MakeNote(Guid("{2}"), L"<td id=\"", 0, tags2));

	noteListModel.Reset();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes[0].html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{0}\"/></div></td><td>Note</td></tr><tr><td>tag-0, tag-1</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[1].html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{1}\"/></div></td><td></td></tr><tr><td></td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[2].html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{2}\"/></div></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;, &lt;strong&gt;not bold&lt;/strong</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_AnonymousUserLoaded_Test)
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	userModel.SetCredentials(L"[anonymous]", L"");
	userModel.notes.push_back(MakeNote(L"note-0", 0));
	userModel.notes.push_back(MakeNote(L"note-1", 0));

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");

	BOOST_CHECK_EQUAL(noteListView.profileText, L"Profile");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign in");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, false);
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_NamedUserLoaded_Test)
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	userModel.SetCredentials(L"test-usr", L"");
	userModel.notes.push_back(MakeNote(L"note-0", 0));
	userModel.notes.push_back(MakeNote(L"note-1", 0));

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");

	BOOST_CHECK_EQUAL(noteListView.profileText, L"test-usr");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign out");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, true);
}

BOOST_AUTO_TEST_CASE(NotListPresenter_LoadThumbnail_Test)
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	userModel.notes.push_back(MakeNote(Guid("{0}"), L"note-0", 0));
	userModel.notes.push_back(MakeNote(Guid("{1}"), L"note-1", 0));

	Blob * blob(NULL);
	noteListView.SignalLoadThumbnail(Guid("{1}"), blob);

	BOOST_CHECK(blob != NULL);
	BOOST_CHECK_EQUAL(noteView.renderSize.cx, 164);
	BOOST_CHECK_EQUAL(noteView.renderSize.cy, 100);
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_UpdateNotebookList_Test)
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-0";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-1";

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0), L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1), L"notebook-1");
}
