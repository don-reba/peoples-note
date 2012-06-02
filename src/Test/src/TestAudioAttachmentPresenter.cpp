#include "stdafx.h"
#include "AudioAttachmentPresenter.h"

#include "MockAudioPlayer.h"
#include "MockAudioPlayerView.h"
#include "MockCredentialsModel.h"
#include "MockNoteView.h"
#include "MockSqlBlob.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct AudioAttachmentPresenterFixture
{
	MockAudioPlayer      audioPlayer;
	MockAudioPlayerView  audioPlayerView;
	MockCredentialsModel credentialsModel;
	MockNoteView         noteView;
	MockUserModel        userModel;

	AudioAttachmentPresenter presenter;

	AudioAttachmentPresenterFixture()
		: presenter
			( audioPlayer
			, audioPlayerView
			, credentialsModel
			, noteView
			, userModel
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_Cancel
	, AudioAttachmentPresenterFixture
	)
{
	audioPlayerView.isShown = true;
	audioPlayerView.SignalCancel();
	BOOST_CHECK(!audioPlayerView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_Hide
	, AudioAttachmentPresenterFixture
	)
{
	audioPlayer.isPlaying = true;
	userModel.loadMethod = MockUserModel::LoadMethodLoad;
	audioPlayerView.SignalHide();
	BOOST_CHECK(!audioPlayer.isPlaying);
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);
}

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_Play
	, AudioAttachmentPresenterFixture
	)
{
	// empty GUID

	audioPlayer.isPlaying = false;

	noteView.selectedAttachment.Guid = Guid::GetEmpty();

	boost::shared_ptr<MockSqlBlob> sqlBlob(make_shared<MockSqlBlob>());
	sqlBlob->data.push_back(7);
	userModel.resourceData = sqlBlob;

	audioPlayerView.SignalPlay();

	BOOST_CHECK(!audioPlayer.isPlaying);
	BOOST_CHECK(!audioPlayer.data);

	// non-empty GUID

	noteView.selectedAttachment.Guid = Guid("test");

	audioPlayerView.SignalPlay();

	BOOST_CHECK(audioPlayer.isPlaying);
	BOOST_CHECK(audioPlayer.data);
}

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_PlayAttachment
	, AudioAttachmentPresenterFixture
	)
{
	audioPlayerView.isShown = false;

	noteView.selectedAttachment.Text = L"test";

	noteView.SignalPlayAttachment();

	BOOST_CHECK(audioPlayerView.isShown);
	BOOST_CHECK_EQUAL(audioPlayerView.name, L"test");
}

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_Show
	, AudioAttachmentPresenterFixture
	)
{
	credentialsModel.username = L"test-user";

	audioPlayerView.SignalShow();

	BOOST_CHECK_EQUAL(userModel.loadedAs, L"test-user");
}

BOOST_FIXTURE_TEST_CASE
	( AudioAttachmentPresenter_Stop
	, AudioAttachmentPresenterFixture
	)
{
	audioPlayer.isPlaying = true;

	audioPlayerView.SignalStop();

	BOOST_CHECK(!audioPlayer.isPlaying);
}
