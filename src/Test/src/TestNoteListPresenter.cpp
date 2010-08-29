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

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"";
	userModel.notes.back().guid = Guid("{1}");
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"<td id=\"";
	userModel.notes.back().guid = Guid("{2}");
	userModel.notes.back().isDirty = true;

	copy
		( userModel.notes.begin()
		, userModel.notes.end()
		, back_inserter(noteListModel.notes)
		);

	noteListModel.hasPreviousNotes = false;
	noteListModel.hasNextNotes     = true;

	noteListModel.SignalChanged();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(0).html
		, L"<img #thumb src=\"thumb:{0}\"/><text #time>1969-12-31 18:00</text><text #title>Note</text>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(1).html
		, L"<img #thumb src=\"thumb:{1}\"/><text #time>1969-12-31 18:00</text><text #title></text>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(2).html
		, L"<img #thumb src=\"thumb:{2}\"/><text #time>1969-12-31 18:00</text><text #title>&lt;td id=&quot;</text>"
		);

	BOOST_CHECK_EQUAL(noteListView.syncText, L"3");

	BOOST_CHECK(noteListView.isPageDownVisible);
	BOOST_CHECK(!noteListView.isPageUpVisible);
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

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
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

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
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

	noteListView.notebooks.push_back(MockNoteListView::NotebookRecord());
	noteListView.notebooks.back().html = L"fake-notebook";

	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-0";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-1";

	syncModel.SignalNotebooksChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");

	BOOST_CHECK_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0).html, L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1).html, L"notebook-1");

	BOOST_CHECK(noteListView.notebooksUpdated);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebookSelected_Test
	, NoteListPresenterFixture
	)
{
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"notebook-0";
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

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"notebook-0");
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
	( NoteListPresenter_SyncBegin_Test
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
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(0).html, L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks.at(1).html, L"notebook-1");
}
