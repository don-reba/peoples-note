#include "stdafx.h"

#include "MockEnImporter.h"
#include "MockNote.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "EnImportPresenter.h"

#include <algorithm>

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(EnImortPresenter_Import_Test0)
{
	MockEnImporter    enImporter;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;

	EnImportPresenter enImportPresenter
		( enImporter
		, noteListModel
		, noteListView
		, userModel
		);

	noteListModel.notes.push_back
		( new MockNote(Guid(), L"note-0", MockTimestamp(L"", 0))
		);

	vector<MockNote> notes;
	notes.push_back(MockNote(Guid(), L"note-1", MockTimestamp(L"", 1)));
	notes.push_back(MockNote(Guid(), L"note-2", MockTimestamp(L"", 2)));
	copy(notes.begin(), notes.end(), back_inserter(enImporter.notes));

	userModel.lastUsedNotebook = MockNotebook(L"notebook");

	noteListView.hasEnexPath = true;
	noteListView.enexPath    = L"data\\Mixed.enex";

	noteListView.SignalImport();

	BOOST_REQUIRE_EQUAL(userModel.addedNotes.size(), 2);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).first, L"note-1");
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(1).first, L"note-2");
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).second, L"notebook");
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(1).second, L"notebook");

	// should actually be 3
	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).GetTitle(), L"note-1");
	BOOST_CHECK_EQUAL(noteListModel.notes.at(1).GetTitle(), L"note-2");
}

BOOST_AUTO_TEST_CASE(EnImortPresenter_Import_Test1)
{
	MockEnImporter    enImporter;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;

	EnImportPresenter enImportPresenter
		( enImporter
		, noteListModel
		, noteListView
		, userModel
		);

	vector<MockNote> notes;
	notes.push_back(MockNote(Guid(), L"note-0", MockTimestamp()));
	notes.push_back(MockNote(Guid(), L"note-1", MockTimestamp()));
	copy(notes.begin(), notes.end(), back_inserter(enImporter.notes));

	userModel.lastUsedNotebook = MockNotebook(L"notebook");

	noteListView.hasEnexPath = false;
	noteListView.enexPath    = L"test-path";

	noteListView.SignalImport();

	BOOST_REQUIRE_EQUAL(userModel.addedNotes.size(), 0);
}
