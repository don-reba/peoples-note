#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

struct NotePresenterFixture
{
	EnNoteTranslator enNoteTranslator;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;

	NotePresenter notePresenter;

	NotePresenterFixture()
		: notePresenter
			( noteListModel
			, noteListView
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
		, 
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><en-todo checked=\"false\"/></en-note>"
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
}

BOOST_FIXTURE_TEST_CASE
	( NotePresenter_OpenNote_Test
	, NotePresenterFixture
	)
{

	userModel.tags.push_back(Tag());
	userModel.tags.back().guid = Guid("{1}");
	userModel.tags.back().name = L"tag-0";

	userModel.tags.push_back(Tag());
	userModel.tags.back().guid = Guid("{2}");
	userModel.tags.back().name = L"tag-1";

	userModel.notes.push_back(Note());
	userModel.notes.back().guid = Guid("{0}");
	userModel.notes.back().name = L"note-title";

	noteListView.selectedNoteGuid = Guid("{0}");
	userModel.noteBodies["{0}"] = L"<en-note>test-note</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL(noteView.body, L"<div type=\"en-note\">test-note</div>");
	BOOST_CHECK_EQUAL(noteView.title,    L"note-title");
	BOOST_CHECK_EQUAL(noteView.subtitle, L"created on 1969-12-31 18:00");
	BOOST_CHECK(noteView.isShown);

	userModel.noteTags.insert(MockUserModel::NoteTag("{0}", "{1}"));
	userModel.noteTags.insert(MockUserModel::NoteTag("{0}", "{2}"));

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL(noteView.body, L"<div type=\"en-note\">test-note</div>");
	BOOST_CHECK_EQUAL(noteView.title,    L"note-title");
	BOOST_CHECK_EQUAL(noteView.subtitle, L"created on 1969-12-31 18:00\ntags: tag-0, tag-1");
	BOOST_CHECK(noteView.isShown);
}
