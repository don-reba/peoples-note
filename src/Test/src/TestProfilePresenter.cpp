#include "stdafx.h"
#include "ProfilePresenter.h"

#include "DbLocation.h"
#include "MockProfileView.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"

struct ProfilePresenterFixture
{
	MockProfileView  profileView;
	MockNoteListView noteListView;
	MockUserModel    userModel;

	ProfilePresenter profilePresenter;

	ProfilePresenterFixture()
		: profilePresenter
			( profileView
			, noteListView
			, userModel
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_Close
	, ProfilePresenterFixture
	)
{
	profileView.isShown = true;

	profileView.SignalClose();

	BOOST_CHECK(!profileView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_DbMove
	, ProfilePresenterFixture
	)
{
	userModel.location = DbLocationNone;
	userModel.path     = L"path";

	profileView.SignalDbMove();

	BOOST_CHECK_EQUAL(userModel.path, L"device-path");
	BOOST_CHECK_EQUAL(userModel.location, DbLocationDevice);

	profileView.SignalDbMove();

	BOOST_CHECK_EQUAL(userModel.path, L"card-path");
	BOOST_CHECK_EQUAL(userModel.location, DbLocationCard);

	profileView.SignalDbMove();

	BOOST_CHECK_EQUAL(userModel.path, L"device-path");
	BOOST_CHECK_EQUAL(userModel.location, DbLocationDevice);
}

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_Loaded
	, ProfilePresenterFixture
	)
{
	// window hidden
	userModel.path     = L"device-path";
	userModel.location = DbLocationDevice;

	profileView.isShown        = false;
	profileView.dbPath         = L"";
	profileView.moveButtonText = L"";

	userModel.SignalLoaded();

	BOOST_CHECK_EQUAL(profileView.dbPath,         L"");
	BOOST_CHECK_EQUAL(profileView.moveButtonText, L"");

	// loaded from device memory
	profileView.isShown = true;

	userModel.SignalLoaded();

	BOOST_CHECK_EQUAL(profileView.dbPath,         L"device-path");
	BOOST_CHECK_EQUAL(profileView.moveButtonText, L"Move to storage card");

	// loaded from storage card
	userModel.location = DbLocationCard;

	userModel.SignalLoaded();

	BOOST_CHECK_EQUAL(profileView.moveButtonText, L"Move to device memory");

	// database closed
	profileView.moveButtonText = L"";
	userModel.location         = DbLocationNone;

	userModel.SignalLoaded();

	BOOST_CHECK_EQUAL(profileView.moveButtonText, L"");
}

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_Profile
	, ProfilePresenterFixture
	)
{
	profileView.isShown = false;
	userModel.location  = DbLocationDevice;

	userModel.credentials.SetUsername(L"username");
	userModel.credentials.SetPassword(L"password");

	userModel.path = L"db-path";
	userModel.size = 235;

	noteListView.SignalProfile();

	BOOST_CHECK(profileView.isShown);
	BOOST_CHECK_EQUAL
		( profileView.moveButtonText
		, L"Move to storage card"
		);
	BOOST_CHECK_EQUAL(profileView.username, L"username");
	BOOST_CHECK_EQUAL(profileView.dbPath,   L"db-path");
	BOOST_CHECK_EQUAL(profileView.dbSize,   L"235 B");
}
