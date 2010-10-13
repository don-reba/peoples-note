#include "stdafx.h"
#include "PhotoEditorPresenter.h"

#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockPhotoEditorView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct PhotoEditorPresenterFixture
{
	MockNoteListModel   noteListModel;
	MockNoteListView    noteListView;
	MockPhotoEditorView photoEditorView;
	MockUserModel       userModel;

	PhotoEditorPresenter presenter;

	PhotoEditorPresenterFixture()
		: presenter
			( noteListModel
			, noteListView
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
	( PhotoEditorPresenter_Capture
	, PhotoEditorPresenterFixture
	)
{
	photoEditorView.isShown = true;
	photoEditorView.title   = L"test title";
	photoEditorView.image.push_back(2);
	photoEditorView.image.push_back(3);

	photoEditorView.SignalCapture();

	BOOST_CHECK(!photoEditorView.isShown);

	BOOST_REQUIRE_EQUAL(userModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).name,    L"test title");
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
	photoEditorView.isShown = false;

	noteListView.SignalNewPhotoNote();
	
	BOOST_CHECK(photoEditorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorPresenter_Ok
	, PhotoEditorPresenterFixture
	)
{
	photoEditorView.isCaptureInitiated = false;

	photoEditorView.SignalOk();
	
	BOOST_CHECK(photoEditorView.isCaptureInitiated);
}
