#include "stdafx.h"
#include "EnNoteTranslator.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "MockSyncModel.h"
#include "NoteListPresenter.h"
#include "Note.h"
#include "Tag.h"
#include "MockUserModel.h"

#include <algorithm>

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct NoteListPresenterFixture
{
	EnNoteTranslator  enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockSyncModel     syncModel;
	MockUserModel     userModel;
	NoteListPresenter presenter;

	NoteListPresenterFixture()
		: presenter
			( noteListModel
			, noteListView
			, noteView
			, userModel
			, syncModel
			, enNoteTranslator
			)
	{
	}
};

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

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NoteListChanged_Test
	, NoteListPresenterFixture
	)
{

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"Note";
	userModel.notes.back().guid = Guid("{0}");
	userModel.notes.back().isDirty = true;

	userModel.notes.back().tags.push_back(Tag());
	userModel.notes.back().tags.back().name = L"tag-0";
	userModel.notes.back().tags.push_back(Tag());
	userModel.notes.back().tags.back().name = L"tag-1";

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"";
	userModel.notes.back().guid = Guid("{1}");
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"<td id=\"";
	userModel.notes.back().guid = Guid("{2}");
	userModel.notes.back().isDirty = true;

	userModel.notes.back().tags.push_back(Tag());
	userModel.notes.back().tags.back().name = L"&amp;";
	userModel.notes.back().tags.push_back(Tag());
	userModel.notes.back().tags.back().name = L"<strong>not bold</strong";

	copy
		( userModel.notes.begin()
		, userModel.notes.end()
		, back_inserter(noteListModel.notes)
		);

	noteListModel.SignalChanged();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(0).html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{0}\"/></div></td><td>Note</td></tr><tr><td>tag-0, tag-1</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(1).html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{1}\"/></div></td><td></td></tr><tr><td></td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(2).html
		, L"<table><tr><td rowspan=\"3\"><div id=\"thumb\"><img width=\"164\" height=\"100\" src=\"thumb:{2}\"/></div></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;, &lt;strong&gt;not bold&lt;/strong</td></tr><tr><td>1970-01-01 00:00</td></tr></table>"
		);

	BOOST_CHECK_EQUAL(noteListView.syncText, L"3");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_AnonymousUserLoaded_Test
	, NoteListPresenterFixture
	)
{
	userModel.SetCredentials(L"[anonymous]", L"");

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-0";

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-1";

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");

	BOOST_CHECK_EQUAL(noteListView.profileText, L"Profile");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign in");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, false);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NamedUserLoaded_Test
	, NoteListPresenterFixture
	)
{
	userModel.SetCredentials(L"test-usr", L"");
	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-0";

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-1";

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");

	BOOST_CHECK_EQUAL(noteListView.profileText, L"test-usr");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign out");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, true);
}

BOOST_FIXTURE_TEST_CASE
	( NotListPresenter_LoadThumbnail_Test
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().guid = Guid("{0}");
	userModel.notes.push_back(Note());
	userModel.notes.back().guid = Guid("{1}");

	Blob * blob(NULL);
	noteListView.SignalLoadThumbnail(Guid("{1}"), blob);

	BOOST_CHECK(blob != NULL);
	BOOST_CHECK_EQUAL(noteView.renderSize.cx, 164);
	BOOST_CHECK_EQUAL(noteView.renderSize.cy, 100);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebooksChanged_Test
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	noteListView.notebooks.push_back(L"fake-notebook");

	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-0";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-1";

	syncModel.SignalNotebooksChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");

	BOOST_CHECK_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0), L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1), L"notebook-1");

	BOOST_CHECK(noteListView.notebooksUpdated);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebookSelected_Test
	, NoteListPresenterFixture
	)
{
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().guid = noteListView.selectedNotebookGuid;

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-0";
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-1";
	userModel.notes.back().isDirty = true;

	noteListView.SignalNotebookSelected();

	BOOST_CHECK_EQUAL
		( userModel.lastUsedNotebook.guid
		, noteListView.selectedNotebookGuid
		);

	BOOST_CHECK_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotesChanged_Test
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().name    = L"note-0";
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().name    = L"note-1";
	userModel.notes.back().isDirty = true;

	syncModel.SignalNotesChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");

	BOOST_CHECK_EQUAL
		( userModel.notebookSelection
		, L"last-used-notebook"
		);

	BOOST_CHECK_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).name, L"note-1");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_SincBegin_Test
	, NoteListPresenterFixture
	)
{
	noteListView.isSyncEnabled = true;

	noteListView.SignalSync();

	BOOST_CHECK(!noteListView.isSyncEnabled);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_SyncEnd_Test
	, NoteListPresenterFixture
	)
{
	noteListView.isSyncEnabled = false;

	syncModel.SignalSyncComplete();

	BOOST_CHECK(noteListView.isSyncEnabled);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_TagsChanged_Test
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	syncModel.SignalTagsChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_UpdateNotebookList_Test
	, NoteListPresenterFixture
	)
{
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-0";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-1";

	userModel.SignalLoaded();

	BOOST_REQUIRE_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0), L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1), L"notebook-1");
}
