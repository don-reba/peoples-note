#include "stdafx.h"
#include "MockProfileView.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "ProfilePresenter.h"

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
	( ProfilePresenter_MoveToCard
	, ProfilePresenterFixture
	)
{
	userModel.path              = L"path";
	profileView.dbPath          = L"";
	profileView.moveButtonState = IProfileView::MoveButtonCard;

	profileView.SignalMoveToCard();

	BOOST_CHECK_EQUAL(userModel.path, L"card-path");
	BOOST_CHECK_EQUAL(profileView.dbPath, L"card-path");
	BOOST_CHECK_EQUAL(profileView.moveErrorMessage, L"");
	BOOST_CHECK_EQUAL
		( profileView.moveButtonState
		, IProfileView::MoveButtonDevice
		);
}

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_MoveToDevice
	, ProfilePresenterFixture
	)
{
	userModel.path              = L"path";
	profileView.dbPath          = L"";
	profileView.moveButtonState = IProfileView::MoveButtonDevice;

	profileView.SignalMoveToDevice();

	BOOST_CHECK_EQUAL(userModel.path, L"device-path");
	BOOST_CHECK_EQUAL(profileView.dbPath, L"device-path");
	BOOST_CHECK_EQUAL(profileView.moveErrorMessage, L"");
	BOOST_CHECK_EQUAL
		( profileView.moveButtonState
		, IProfileView::MoveButtonCard
		);
}

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_Profile
	, ProfilePresenterFixture
	)
{
	profileView.isShown = false;
	profileView.moveButtonState = IProfileView::MoveButtonDevice;

	userModel.credentials.SetUsername(L"username");
	userModel.credentials.SetPassword(L"password");

	userModel.path = L"db-path";
	userModel.size = 235;

	noteListView.SignalProfile();

	BOOST_CHECK(profileView.isShown);
	BOOST_CHECK_EQUAL
		( profileView.moveButtonState
		, IProfileView::MoveButtonCard
		);
	BOOST_CHECK_EQUAL(profileView.username, L"username");
	BOOST_CHECK_EQUAL(profileView.dbPath,   L"db-path");
	BOOST_CHECK_EQUAL(profileView.dbSize,   L"235 B");
}
