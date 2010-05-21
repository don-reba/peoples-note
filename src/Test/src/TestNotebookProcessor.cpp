#include "stdafx.h"
#include "NotebookProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Notebook.h"

#include <algorithm>

using namespace std;

struct NotebookProcessorFixture
{
	MockUserModel userModel;

	NotebookProcessor notebookProcessor;

	NotebookProcessorFixture()
		: notebookProcessor (userModel)
	{
	}
};

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Add_Test, NotebookProcessorFixture)
{
	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.Add(notebook);

	BOOST_CHECK_EQUAL(userModel.notebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notebooks.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Delete_Test, NotebookProcessorFixture)
{
	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.Delete(notebook);

	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Rename_Test, NotebookProcessorFixture)
{
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

	notebookProcessor.RenameAdd(userModel.notebooks.at(0), notebook);

	BOOST_CHECK_EQUAL(userModel.notebooks.size(), 5);
	BOOST_CHECK_EQUAL(userModel.notebooks.at(0).name, L"test(4)");
	BOOST_CHECK_EQUAL(userModel.notebooks.at(4).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Upload_Test, NotebookProcessorFixture)
{
	MockNoteStore noteStore;

	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.Upload(notebook, noteStore);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name, L"test");
}
