#include "stdafx.h"

#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

BOOST_AUTO_TEST_CASE(NotePresenterLoadingData_Test)
{
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter(noteListView, noteView, userModel);

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

BOOST_AUTO_TEST_CASE(NotePresenterMedia_Test)
{
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter(noteListView, noteView, userModel);

	Guid guid;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] =
		L"<en-note>"
		L"note"
		L"<en-media border=\"1\" hash=\"d978\" type=\"image/jpeg\" />"
		L"</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL
		( noteView.body
		, L"<en-note>"
			L"note"
			L"<img src=\"img:d978\" border=\"1\"/>"
			L"</en-note>"
		);
	BOOST_CHECK(noteView.isShown);
}

BOOST_AUTO_TEST_CASE(NotePresenterTodo_Test)
{
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter(noteListView, noteView, userModel);

	Guid guid;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] =
		L"<en-note>"
		L"note"
		L"<en-todo checked=\"true\" />"
		L"<en-todo checked=\"false\" />"
		L"</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL
		( noteView.body
		, L"<en-note>"
			L"note"
			L"<input type=\"checkbox\" checked=\"\"/>"
			L"<input type=\"checkbox\"/>"
			L"</en-note>"
		);
	BOOST_CHECK(noteView.isShown);
}
