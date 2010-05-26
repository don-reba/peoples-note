#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

struct NotePresenterFixture
{
	EnNoteTranslator enNoteTranslator;
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;

	NotePresenter notePresenter;

	NotePresenterFixture()
		: notePresenter
			( noteListView
			, noteView
			, userModel
			, enNoteTranslator
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( NotePresenter_CloseNote_Test
	, NotePresenterFixture
	)
{
	Guid guid;
	userModel.notes.push_back(Note());
	userModel.notes.back().guid    = guid;
	userModel.notes.back().isDirty = false;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"<en-note/>";

	noteListView.SignalOpenNote();

	noteView.isDirty = true;
	noteView.body =
		L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";

	noteView.SignalClose();

	BOOST_CHECK_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.isDirty, true);

	BOOST_CHECK_EQUAL
		( userModel.addedNotes.at(0).body
		, L"<en-note><en-todo checked=\"false\"/></en-note>"
		);

	BOOST_CHECK_EQUAL(noteView.renderSize.cx, 164);
	BOOST_CHECK_EQUAL(noteView.renderSize.cy, 100);

	bool isThumbnailUpdated
		(  noteListView.updatedThumbnails.find(guid)
		!= noteListView.updatedThumbnails.end()
		);
	BOOST_CHECK(isThumbnailUpdated);
}

BOOST_FIXTURE_TEST_CASE
	( NotePresenter_LoadingData_Test
	, NotePresenterFixture
	)
{
	userModel.resources.push_back(Resource());
	userModel.resources.back().Data.push_back(2);
	userModel.resources.back().Data.push_back(3);
	userModel.resources.back().Data.push_back(5);
	userModel.resources.back().Hash = "test";

	Blob result;
	noteView.SignalLoadingData(L"img:test", result);
	BOOST_REQUIRE_EQUAL(result.size(), 3);
	BOOST_CHECK_EQUAL(result.at(0), 2);
	BOOST_CHECK_EQUAL(result.at(1), 3);
	BOOST_CHECK_EQUAL(result.at(2), 5);
}

BOOST_FIXTURE_TEST_CASE
	( NotePresenter_OpenNote_Test
	, NotePresenterFixture
	)
{
	Guid guid;
	userModel.notes.push_back(Note());
	userModel.notes.back().guid = guid;
	userModel.notes.back().name = L"note-title";
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"<en-note>test-note</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL(noteView.body, L"<div type=\"en-note\">test-note</div>");
	BOOST_CHECK_EQUAL(noteView.title,    L"note-title");
	BOOST_CHECK_EQUAL(noteView.subtitle, L"created on 1970-01-01 00:00");
	BOOST_CHECK(noteView.isShown);
}
