#include "stdafx.h"
#include "MockNoteListView.h"
#include "MockSyncModel.h"
#include "MockLastUserModel.h"
#include "SyncPresenter.h"

using namespace boost;
using namespace std;

struct SyncPresenterFixture
{
	MockNoteListView  noteListView;
	MockSyncModel     syncModel;
	MockLastUserModel lastUserModel;
	SyncPresenter     syncPresenter;

	SyncPresenterFixture()
		: syncPresenter
			( noteListView
			, syncModel
			, lastUserModel
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( SyncPresenter_Sync
	, SyncPresenterFixture
	)
{
	lastUserModel.username = L"username";
	lastUserModel.password = L"password";

	noteListView.SignalSync();

	BOOST_CHECK(syncModel.syncBegan);
	BOOST_CHECK_EQUAL(syncModel.username, L"username");
	BOOST_CHECK_EQUAL(syncModel.password, L"password");
}
