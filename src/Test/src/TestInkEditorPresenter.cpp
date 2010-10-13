#include "stdafx.h"
#include "InkEditorPresenter.h"

#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockInkEditorView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct InkEditorPresenterFixture
{
	MockInkEditorView InkEditorView;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;

	InkEditorPresenter presenter;

	InkEditorPresenterFixture()
		: presenter
			( InkEditorView
			, noteListModel
			, noteListView
			, userModel
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( InkEditorPresenter_Cancel
	, InkEditorPresenterFixture
	)
{
	InkEditorView.isShown = true;

	InkEditorView.SignalCancel();
	
	BOOST_CHECK(!InkEditorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( InkEditorPresenter_Accept
	, InkEditorPresenterFixture
	)
{
	InkEditorView.isShown = true;
	InkEditorView.image.push_back(2);
	InkEditorView.image.push_back(3);

	InkEditorView.SignalAccept();

	BOOST_CHECK(!InkEditorView.isShown);

	BOOST_REQUIRE_EQUAL(userModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).name,    L"Ink note");
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
	( InkEditorPresenter_NewInk
	, InkEditorPresenterFixture
	)
{
	InkEditorView.isShown = false;

	noteListView.SignalNewInkNote();
	
	BOOST_CHECK(InkEditorView.isShown);
}
