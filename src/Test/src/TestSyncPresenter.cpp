#include "stdafx.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockSyncModel.h"
#include "MockUserModel.h"
#include "SyncPresenter.h"

using namespace boost;
using namespace std;

struct SyncPresenterFixture
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockSyncModel     syncModel;
	MockUserModel     userModel;
	SyncPresenter     syncPresenter;

	SyncPresenterFixture()
		: syncPresenter
			( noteListModel
			, noteListView
			, syncModel
			, userModel
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( SyncPresenter_Sync
	, SyncPresenterFixture
	)
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

	userModel.credentials.SetUsername(L"test-usr");

	noteListView.SignalSync();

	BOOST_CHECK(syncModel.syncBegan);
	BOOST_CHECK_EQUAL(syncModel.username, L"test-usr");
}

BOOST_FIXTURE_TEST_CASE
	( SyncPresenter_SyncComplete
	, SyncPresenterFixture
	)
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

	userModel.notes.push_back(Note());
	userModel.notes.back().name = L"test-note";

	syncModel.SignalSyncComplete();

	BOOST_CHECK_EQUAL(userModel.notebookSelection, L"last-used-notebook");

	BOOST_REQUIRE_EQUAL(noteListModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(noteListModel.notes.at(0).name, L"test-note");
}
