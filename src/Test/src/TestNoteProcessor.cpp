#include "stdafx.h"
#include "NoteProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Notebook.h"

using namespace std;

BOOST_AUTO_TEST_CASE(NoteProcessor_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;
	Notebook notebook(Guid(), L"test-notebook");

	NoteProcessor noteProcessor
		( noteStore
		, userModel
		, notebook
		);
}
