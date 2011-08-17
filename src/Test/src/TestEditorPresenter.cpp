#include "stdafx.h"
#include "EditorPresenter.h"

#include "MockEditorView.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "MockUserModel.h"
#include "EnNoteTranslator.h"
#include "Thumbnail.h"

struct EditorPresenterFixture
{
	MockEditorView    editorView;
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockNoteView      noteView;
	MockUserModel     userModel;
	
	EnNoteTranslator enNoteTranslator;

	EditorPresenter editorPresenter;

	EditorPresenterFixture()
		: editorPresenter
			( editorView
			, noteListModel
			, noteListView
			, noteView
			, userModel
			, enNoteTranslator
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_Accept
	, EditorPresenterFixture
	)
{
	editorView.isShown = true;

	noteListView.thumbSize.cx = 164;
	noteListView.thumbSize.cy = 100;

	Thumbnail thumbnail;
	thumbnail.Width  = 0;
	thumbnail.Height = 0;
	userModel.noteThumbnails[editorView.note.guid] = thumbnail;

	noteListView.SignalNewNote();

	editorView.body =
		L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	editorView.note.name = L"New Title";

	editorView.SignalAccept();

	BOOST_CHECK(!editorView.isShown);

	BOOST_CHECK_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.isDirty, true);

	BOOST_CHECK(userModel.addedNotes.at(0).note.modificationDate.GetTime() > 0);

	BOOST_CHECK_EQUAL
		( userModel.addedNotes.at(0).body
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><en-todo checked=\"false\"/></en-note>"
		);
	BOOST_CHECK_EQUAL
		( userModel.addedNotes.at(0).note.name
		, L"New Title"
		);

	BOOST_CHECK
		(  userModel.noteThumbnails.find(editorView.note.guid)
		== userModel.noteThumbnails.end()
		);

	BOOST_CHECK(noteListModel.isReloaded);
}

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_Cancel
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
	editorView.note.name = L"New Title";

	editorView.SignalCancel();

	BOOST_CHECK(!editorView.isShown);
	BOOST_CHECK(userModel.addedNotes.empty());
	BOOST_CHECK(noteListView.updatedThumbnails.empty());
}

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_DeleteNote
	, EditorPresenterFixture
	)
{
	userModel.notes.push_back(editorView.note);

	editorView.SignalDeleteNote();

	BOOST_REQUIRE_EQUAL(userModel.deletedNotes.size(), 1);
	BOOST_CHECK_EQUAL
		( userModel.deletedNotes.at(0)
		, editorView.note.guid
		);

	BOOST_CHECK(!editorView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( EditorPresenter_EditNote
	, EditorPresenterFixture
	)
{
	noteView.isShown   = true;
	editorView.isShown = false;

	noteView.body  = L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	noteView.note.name = L"Note Title";

	noteView.SignalEdit();

	BOOST_CHECK_EQUAL
		( editorView.body
		, L"<div type=\"en-note\"><input type=\"checkbox\"/></div>"
		);
	BOOST_CHECK_EQUAL(editorView.note.name, L"Note Title");

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
	BOOST_CHECK_EQUAL(editorView.body, L"<div type=\"en-note\" />");
	BOOST_CHECK_EQUAL(editorView.note.isDirty, true);
	BOOST_CHECK_EQUAL(editorView.note.usn,     0);
	BOOST_CHECK_EQUAL
		( editorView.note.name
		, L"New note in last-used-notebook"
		);
	BOOST_CHECK_NE
		( editorView.note.creationDate.GetTime()
		, 0L
		);
}
