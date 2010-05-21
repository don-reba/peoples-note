#include "stdafx.h"
#if 0
#include "NotebookProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Notebook.h"

#include <algorithm>

using namespace std;

BOOST_AUTO_TEST_CASE(NotebookProcessor_Add_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	NotebookProcessor noteProcessor
		( noteStore
		, userModel
		);

	Notebook notebook;
	notebook.name = L"test";

	noteProcessor.Add(notebook);

	BOOST_CHECK_EQUAL(userModel.notebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notebooks.at(0).name, L"test");
}

BOOST_AUTO_TEST_CASE(NotebookProcessor_Delete_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	NotebookProcessor noteProcessor
		( noteStore
		, userModel
		);

	Notebook notebook;
	notebook.name = L"test";

	noteProcessor.Delete(notebook);

	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.at(0).name, L"test");
}

BOOST_AUTO_TEST_CASE(NotebookProcessor_Rename_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	NotebookProcessor noteProcessor
		( noteStore
		, userModel
		);

	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"test";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"test(1)";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"test(2)";
	userModel.notebooks.push_back(Notebook());
	userModel.notebooks.back().name = L"test(3)";

	Notebook notebook;
	notebook.name = L"test";

	noteProcessor.Rename(notebook);

	BOOST_CHECK_EQUAL(userModel.notebooks.size(), 5);
	BOOST_CHECK_EQUAL(userModel.notebooks.at(4).name, L"test(4)");
}

BOOST_AUTO_TEST_CASE(NotebookProcessor_Upload_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	NotebookProcessor noteProcessor
		( noteStore
		, userModel
		);

	Notebook notebook;
	notebook.name = L"test";

	noteProcessor.Upload(notebook);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name, L"test");
}
#endif // 0