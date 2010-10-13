#include "stdafx.h"
#include "EditorPresenter.h"

#include "MockEditorView.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "MockUserModel.h"
#include "EnNoteTranslator.h"

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
	( EditorPresenter_Accept_Test
	, EditorPresenterFixture
	)
{
	editorView.isShown = true;

	noteListView.thumbSize.cx = 164;
	noteListView.thumbSize.cy = 100;

	noteListView.SignalNewNote();

	editorView.body =
		L"<div type=\"en-note\"><input type=\"checkbox\"/></div>";
	editorView.note.name = L"New Title";

	editorView.SignalAccept();

	BOOST_CHECK(!editorView.isShown);

	BOOST_CHECK_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.isDirty, true);

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

	BOOST_CHECK_EQUAL(noteView.renderSize.cx, 164);
	BOOST_CHECK_EQUAL(noteView.renderSize.cy, 100);

	bool isThumbnailUpdated
		(  userModel.noteThumbnails.find(editorView.note.guid)
		!= userModel.noteThumbnails.end()
		);
	BOOST_CHECK(isThumbnailUpdated);

	BOOST_CHECK(noteListModel.isReloaded);
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
	editorView.note.name = L"New Title";

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
