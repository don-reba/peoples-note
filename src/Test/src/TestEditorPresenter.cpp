#include "stdafx.h"
#include "EditorPresenter.h"

#include "MockEditorView.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "MockUserModel.h"
#include "EnNoteTranslator.h"

struct EditorPresenterFixture
{
	MockEditorView   editorView;
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	
	EnNoteTranslator enNoteTranslator;

	EditorPresenter editorPresenter;

	EditorPresenterFixture()
		: editorPresenter
			( editorView
			, noteListView
			, noteView
			, userModel
			, enNoteTranslator
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_Accept_Test
	, EditorPresenterFixture
	)
{
	editorView.isShown = true;

	Guid guid;
	userModel.notes.push_back(Note());
	userModel.notes.back().guid    = guid;
	userModel.notes.back().isDirty = false;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"<en-note/>";

	noteListView.SignalNewNote();

	editorView.body =
		L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	editorView.title = L"New Title";

	editorView.SignalAccept();

	BOOST_CHECK(!editorView.isShown);

	BOOST_CHECK_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.isDirty, true);

	BOOST_CHECK_EQUAL
		( userModel.addedNotes.at(0).body
		, L"<en-note><en-todo checked=\"false\"/></en-note>"
		);
	BOOST_CHECK_EQUAL
		( userModel.addedNotes.at(0).note.name
		, L"New Title"
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
	( EditorPresenter_Cancel_Test
	, EditorPresenterFixture
	)
{
	editorView.isShown = true;

	Guid guid;
	userModel.notes.push_back(Note());
	userModel.notes.back().guid    = guid;
	userModel.notes.back().isDirty = false;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"<en-note/>";

	noteListView.SignalNewNote();

	editorView.body =
		L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	editorView.title = L"New Title";

	editorView.SignalCancel();

	BOOST_CHECK(!editorView.isShown);
	BOOST_CHECK(userModel.addedNotes.empty());
	BOOST_CHECK(noteListView.updatedThumbnails.empty());
}

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_EditNote_Test
	, EditorPresenterFixture
	)
{
	noteView.isShown   = true;
	editorView.isShown = false;

	noteView.body  = L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	noteView.title = L"Note Title";

	noteView.SignalEdit();

	BOOST_CHECK_EQUAL
		( editorView.body
		, L"<div type=\"en-note\"><input type=\"checkbox\"/></div>"
		);
	BOOST_CHECK_EQUAL(editorView.title, L"Note Title");

	BOOST_CHECK(!noteView.isShown);
	BOOST_CHECK(editorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_NewNote_Test
	, EditorPresenterFixture
	)
{
	editorView.isShown = false;

	noteListView.SignalNewNote();

	BOOST_CHECK(editorView.isShown);
	BOOST_CHECK(editorView.body.empty());
	BOOST_CHECK(editorView.title.empty());
}
