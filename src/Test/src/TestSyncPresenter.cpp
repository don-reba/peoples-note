#include "stdafx.h"
#include "MockNoteListView.h"
#include "MockSyncModel.h"
#include "MockUserModel.h"
#include "SyncPresenter.h"

using namespace boost;
using namespace std;

struct SyncPresenterFixture
{
	MockNoteListView  noteListView;
	MockSyncModel     syncModel;
	MockUserModel     userModel;
	SyncPresenter     syncPresenter;

	SyncPresenterFixture()
		: syncPresenter
			( noteListView
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
	userModel.credentials.SetUsername(L"test-usr");

	noteListView.SignalSync();

	BOOST_CHECK(syncModel.syncBegan);
	BOOST_CHECK_EQUAL(syncModel.username, L"test-usr");
}
