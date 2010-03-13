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
	userModel.noteBodies[guid] =
		L"<en-note>"
		L"note"
		L"<en-media border=\"1\" hash=\"d978\" type=\"image/jpeg\" />"
		L"</en-note>";

	noteListView.SignalOpenNote();

	BOOST_CHECK_EQUAL
		( noteView.body
		, L"<en-note>note<img src=\"img:d978\" border=\"1\"/></en-note>"
		);
	BOOST_CHECK(noteView.isShown);

	Blob blob;
	noteView.SignalLoadingData(L"img:d978", blob);
}
