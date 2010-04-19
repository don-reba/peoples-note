#include "stdafx.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockSyncModel.h"
#include "MockUserModel.h"
#include "SyncPresenter.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(SyncModel_Sync_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockSyncModel     syncModel;
	MockUserModel     userModel;

	SyncPresenter syncPresenter
		( noteListModel
		, noteListView
		, syncModel
		, userModel
		);

	noteListView.SignalSync();

	BOOST_CHECK(syncModel.syncBegan);
}

BOOST_AUTO_TEST_CASE(SyncModel_SyncComplete_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockSyncModel     syncModel;
	MockUserModel     userModel;

	SyncPresenter syncPresenter
		( noteListModel
		, noteListView
		, syncModel
		, userModel
		);

	userModel.lastUsedNotebook = Notebook(Guid(), L"test-notebook");
	userModel.notes.push_back(Note(Guid(), L"test-note", Timestamp(0)));

	syncModel.SignalSyncComplete();

	BOOST_CHECK_EQUAL(userModel.notebookSelection, L"test-notebook");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).GetTitle(), L"test-note");
}
