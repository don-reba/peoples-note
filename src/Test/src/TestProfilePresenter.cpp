#include "stdafx.h"
#include "MockProfileView.h"
#include "MockNoteListView.h"
#include "ProfilePresenter.h"

struct ProfilePresenterFixture
{
	MockProfileView  profileView;
	MockNoteListView noteListView;

	ProfilePresenter profilePresenter;

	ProfilePresenterFixture()
		: profilePresenter
			( profileView
			, noteListView
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( ProfilePresenter_Test
	, ProfilePresenterFixture
	)
{
	noteListView.SignalProfile();

	BOOST_CHECK(profileView.isShown);

	profileView.SignalClose();

	BOOST_CHECK(!profileView.isShown);
}