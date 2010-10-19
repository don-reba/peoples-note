#include "stdafx.h"
#include "InkEditorPresenter.h"

#include "MockInkEditorModel.h"
#include "MockInkEditorView.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct InkEditorPresenterFixture
{
	MockInkEditorModel inkEditorModel;
	MockInkEditorView  inkEditorView;
	MockNoteListModel  noteListModel;
	MockNoteListView   noteListView;
	MockUserModel      userModel;

	InkEditorPresenter presenter;

	InkEditorPresenterFixture()
		: presenter
			( inkEditorModel
			, inkEditorView
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
	inkEditorView.isShown = true;

	inkEditorView.SignalCancel();
	
	BOOST_CHECK(!inkEditorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( InkEditorPresenter_Accept
	, InkEditorPresenterFixture
	)
{
	inkEditorView.isShown = true;
	inkEditorView.image.push_back(2);
	inkEditorView.image.push_back(3);

	inkEditorView.SignalAccept();

	BOOST_CHECK(!inkEditorView.isShown);

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
	( InkEditorPresenter_NewInkNote
	, InkEditorPresenterFixture
	)
{
	inkEditorModel.penWidth = L"2px";
	inkEditorModel.penColor = L"red";

	inkEditorView.isShown = false;

	noteListView.SignalNewInkNote();
	
	BOOST_CHECK(inkEditorView.isShown);

	BOOST_CHECK_EQUAL(inkEditorView.setPenWidth, Pen2px);
	BOOST_CHECK_EQUAL(inkEditorView.setPenColor, PenRed);
}

BOOST_FIXTURE_TEST_CASE
	( InkEditorPresenter_PenChanged
	, InkEditorPresenterFixture
	)
{
	inkEditorView.penWidth = Pen2px;
	inkEditorView.penColor = PenRed;

	inkEditorView.SignalPenChanged();

	BOOST_CHECK_EQUAL(inkEditorModel.penWidth, L"2px");
	BOOST_CHECK_EQUAL(inkEditorModel.penColor, L"red");

	BOOST_CHECK_EQUAL(inkEditorView.setPenWidth, Pen2px);
	BOOST_CHECK_EQUAL(inkEditorView.setPenColor, PenRed);
}
