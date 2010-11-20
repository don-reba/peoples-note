#include "stdafx.h"
#include "PhotoEditorPresenter.h"

#include "MockFile.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockPhotoEditorModel.h"
#include "MockPhotoEditorView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct PhotoEditorPresenterFixture
{
	MockFile             file;
	MockNoteListModel    noteListModel;
	MockNoteListView     noteListView;
	MockPhotoEditorModel photoEditorModel;
	MockPhotoEditorView  photoEditorView;
	MockUserModel        userModel;

	PhotoEditorPresenter presenter;

	PhotoEditorPresenterFixture()
		: presenter
			( file
			, noteListModel
			, noteListView
			, photoEditorModel
			, photoEditorView
			, userModel
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorPresenter_Cancel
	, PhotoEditorPresenterFixture
	)
{
	photoEditorView.isShown = true;

	photoEditorView.SignalCancel();
	
	BOOST_CHECK(!photoEditorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorPresenter_Messages
	, PhotoEditorPresenterFixture
	)
{
	photoEditorView.photoResult = 0;
	photoEditorView.SignalOk();
	BOOST_CHECK_EQUAL(photoEditorView.message, L"");

	photoEditorView.photoResult = 1;
	photoEditorView.SignalOk();
	BOOST_CHECK_EQUAL(photoEditorView.message, L"Photo capture was cancelled.");

	photoEditorView.photoResult = 0x8007000E;
	photoEditorView.SignalOk();
	BOOST_CHECK_EQUAL(photoEditorView.message, L"Not enough memory.");

	photoEditorView.photoResult = 0x80070057;
	photoEditorView.SignalOk();
	BOOST_CHECK_EQUAL(photoEditorView.message, L"Invalid settings.");

	photoEditorView.photoResult = 42;
	photoEditorView.SignalOk();
	BOOST_CHECK_EQUAL(photoEditorView.message, L"What just happened? Error code: 2a.");
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorPresenter_Ok
	, PhotoEditorPresenterFixture
	)
{
	file.exists = true;
	file.data.push_back(2);
	file.data.push_back(3);

	photoEditorView.isShown    = true;
	photoEditorView.title      = L"";
	photoEditorView.path       = L"path";
	photoEditorView.quality    = PhotoQualityLow;
	photoEditorView.resolution = PhotoResolution1M;

	photoEditorView.SignalOk();

	BOOST_CHECK_EQUAL(photoEditorView.photoQuality, 1);
	BOOST_CHECK_EQUAL(photoEditorView.photoWidth,   1280);
	BOOST_CHECK_EQUAL(photoEditorView.photoHeight,  960);

	BOOST_CHECK_EQUAL(photoEditorModel.quality,    L"low");
	BOOST_CHECK_EQUAL(photoEditorModel.resolution, L"1m");

	BOOST_CHECK(!photoEditorView.isShown);

	BOOST_REQUIRE_EQUAL(userModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).name,    L"Photo");
	BOOST_CHECK_EQUAL(userModel.notes.at(0).isDirty, true);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).usn,     0);
	BOOST_CHECK_EQUAL
		( userModel.noteBodies[userModel.notes.at(0).guid]
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note>"
				L"<en-media type=\"image/jpeg\" hash=\"1fab1cffea9b6fd2f36bfb46e182fd77\"/>"
			L"</en-note>"
		);

	BOOST_REQUIRE_EQUAL(userModel.resources.size(), 1);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Hash, "1fab1cffea9b6fd2f36bfb46e182fd77");

	BOOST_REQUIRE_EQUAL(userModel.resources.at(0).Data.size(), 2);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Data.at(0), 2);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Data.at(1), 3);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Note, userModel.notes.at(0).guid);

	BOOST_CHECK(noteListModel.isReloaded);
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorPresenter_NewPhoto
	, PhotoEditorPresenterFixture
	)
{
	photoEditorModel.quality    = L"normal";
	photoEditorModel.resolution = L"2m";

	photoEditorView.isShown = false;

	noteListView.SignalNewPhotoNote();
	
	BOOST_CHECK(photoEditorView.isShown);

	BOOST_CHECK_EQUAL(photoEditorView.quality,    PhotoQualityNormal);
	BOOST_CHECK_EQUAL(photoEditorView.resolution, PhotoResolution2M);
}
