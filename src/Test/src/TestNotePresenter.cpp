#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

BOOST_AUTO_TEST_CASE(NotePresenterLoadingData_Test)
{
	EnNoteTranslator enNoteTranslator;
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter
		( noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	Blob data;
	data.push_back(2);
	data.push_back(3);
	data.push_back(5);
	userModel.images["test"] = data;

	Blob result;
	noteView.SignalLoadingData(L"img:test", result);
	BOOST_REQUIRE_EQUAL(result.size(), 3);
	BOOST_CHECK_EQUAL(result.at(0), 2);
	BOOST_CHECK_EQUAL(result.at(1), 3);
	BOOST_CHECK_EQUAL(result.at(2), 5);
}

BOOST_AUTO_TEST_CASE(NotePresenter_Content_Test)
{
	EnNoteTranslator enNoteTranslator;
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter
		( noteListView
		, noteView
		, userModel
		, enNoteTranslator
		);

	Guid guid;
	userModel.notes.push_back(Note(guid, L"note-title", Timestamp(0)));
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"<en-note>test-note</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL(noteView.body, L"<div>test-note</div>");
	BOOST_CHECK_EQUAL(noteView.title,    L"note-title");
	BOOST_CHECK_EQUAL(noteView.subtitle, L"created on 1970-01-01 00:00");
	BOOST_CHECK(noteView.isShown);
}
