#include "stdafx.h"

#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

BOOST_AUTO_TEST_CASE(NotePresenter_Test)
{
	MockNoteListView noteListView;
	MockNoteView     noteView;
	MockUserModel    userModel;
	NotePresenter notePresenter(noteListView, noteView, userModel);

	Guid guid;
	noteListView.selectedNoteGuid = guid;
	userModel.noteBodies[guid] = L"test-note-body";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL(noteView.body, L"test-note-body");
	BOOST_CHECK(noteView.isShown);
}
