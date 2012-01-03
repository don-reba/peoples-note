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

// Note: the output of this test depends on your time zone.
BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NoteListChanged
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
	userModel.notes.back().isDirty = false;

	copy
		( userModel.notes.begin()
		, userModel.notes.end()
		, back_inserter(noteListModel.notes)
		);

	noteListModel.hasPreviousPage = false;
	noteListModel.hasNextPage     = true;

	noteListModel.SignalChanged();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_REQUIRE_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(0).html
		, L"<img #thumb src=\"thumb:{0}\"/><text #time>1970-01-01 01:00</text><text #title>Note</text>"
		);
	BOOST_CHECK_EQUAL(noteListView.notes.at(0).value,   L"{0}");
	BOOST_CHECK_EQUAL(noteListView.notes.at(0).isDirty, true);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(1).html
		, L"<img #thumb src=\"thumb:{1}\"/><text #time>1970-01-01 01:00</text><text #title></text>"
		);
	BOOST_CHECK_EQUAL(noteListView.notes.at(1).value,   L"{1}");
	BOOST_CHECK_EQUAL(noteListView.notes.at(1).isDirty, true);
	BOOST_CHECK_EQUAL
		( noteListView.notes.at(2).html
		, L"<img #thumb src=\"thumb:{2}\"/><text #time>1970-01-01 01:00</text><text #title>&lt;td id=&quot;</text>"
		);
	BOOST_CHECK_EQUAL(noteListView.notes.at(2).value,   L"{2}");
	BOOST_CHECK_EQUAL(noteListView.notes.at(2).isDirty, false);

	BOOST_CHECK(noteListView.isPageDownVisible);
	BOOST_CHECK(!noteListView.isPageUpVisible);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_AnonymousUserLoaded
	, NoteListPresenterFixture
	)
{
	userModel.SetCredentials(L"[anonymous]", L"");

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-0";

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-1";

	userModel.SignalLoaded();

	BOOST_CHECK(noteListModel.isReloaded);

	BOOST_CHECK_EQUAL(noteListView.profileText, L"Profile");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign in");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, false);
	BOOST_CHECK_EQUAL(noteListView.isNotebookTitleEnabled, false);
	BOOST_CHECK_EQUAL(noteListView.isNotebookTitleVisible, false);

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NamedUserLoaded
	, NoteListPresenterFixture
	)
{
	userModel.SetCredentials(L"test-usr", L"");
	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-0";

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"note-1";

	noteListModel.notebookTitleState = true;

	userModel.SignalLoaded();

	BOOST_CHECK(noteListModel.isReloaded);

	BOOST_CHECK_EQUAL(noteListView.profileText, L"test-usr");
	BOOST_CHECK_EQUAL(noteListView.signinText,  L"Sign out");
	BOOST_CHECK_EQUAL(noteListView.isSyncButtonShown, true);
	BOOST_CHECK_EQUAL(noteListView.isNotebookTitleEnabled, true);
	BOOST_CHECK_EQUAL(noteListView.isNotebookTitleVisible, true);

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebooksChanged
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	userModel.notebooks.resize(2);
	userModel.notebooks.at(0).name = L"notebook-0";
	userModel.notebooks.at(1).name = L"notebook-1";

	syncModel.SignalNotebooksChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");

	BOOST_CHECK_EQUAL(noteListView.notebookMenu.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(0).name, L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(1).name, L"notebook-1");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebookSelected
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
		( userModel.lastUsedNotebook
		, noteListView.selectedNotebookGuid
		);

	BOOST_CHECK(noteListModel.isReloaded);

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotebookTitle
	, NoteListPresenterFixture
	)
{
	noteListModel.notebookTitleState = false;

	noteListView.isNotebookTitleVisible = false;
	noteListView.isNotebookTitleEnabled = false;

	noteListView.SignalNotebookTitle();

	BOOST_CHECK(noteListModel.notebookTitleState);
	BOOST_CHECK(noteListView.isNotebookTitleVisible);
	BOOST_CHECK(noteListView.isNotebookTitleEnabled);

	noteListView.SignalNotebookTitle();

	BOOST_CHECK(!noteListModel.notebookTitleState);
	BOOST_CHECK(!noteListView.isNotebookTitleVisible);
	BOOST_CHECK(!noteListView.isNotebookTitleEnabled);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_NotesChanged
	, NoteListPresenterFixture
	)
{
	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;
	userModel.notes.push_back(Note());
	userModel.notes.back().isDirty = true;

	syncModel.SignalNotesChanged();

	BOOST_CHECK_EQUAL(noteListView.syncText, L"2");

	BOOST_CHECK(noteListModel.isReloaded);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_SyncBegin
	, NoteListPresenterFixture
	)
{
	noteListView.isSyncEnabled = true;

	noteListView.SignalSync();

	BOOST_CHECK(!noteListView.isSyncEnabled);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_SyncEnd
	, NoteListPresenterFixture
	)
{
	userModel.notebooks.resize(2);
	userModel.notebooks.at(0).name = L"notebook-0";
	userModel.notebooks.at(1).name = L"notebook-1";

	noteListView.isSyncEnabled = false;
	noteListView.syncText      = L"";

	syncModel.SignalSyncComplete();

	BOOST_CHECK_EQUAL(noteListView.windowTitle, L"last-used-notebook");
	BOOST_CHECK_EQUAL(noteListView.syncText,    L"0");

	BOOST_CHECK_EQUAL(noteListView.notebookMenu.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(0).name, L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(1).name, L"notebook-1");

	BOOST_CHECK(noteListView.isSyncEnabled);
}

BOOST_FIXTURE_TEST_CASE
	( NoteListPresenter_TagsChanged
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
	( NoteListPresenter_UpdateNotebookList
	, NoteListPresenterFixture
	)
{
	userModel.notebooks.resize(2);
	userModel.notebooks.at(0).name = L"notebook-0";
	userModel.notebooks.at(1).name = L"notebook-1";

	userModel.SignalLoaded();

	BOOST_CHECK_EQUAL(noteListView.notebookMenu.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(0).name, L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebookMenu.at(1).name, L"notebook-1");
}
