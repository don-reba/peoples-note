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

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Create_Test, NotebookProcessorFixture)
{
	MockNoteStore noteStore;

	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.Create(notebook, noteStore);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Delete_Test, NotebookProcessorFixture)
{
	Notebook notebook;

	notebookProcessor.Delete(notebook);

	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedNotebooks.at(0), notebook.guid);
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_RenameAdd_Test, NotebookProcessorFixture)
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

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_Update_Test, NotebookProcessorFixture)
{
	MockNoteStore noteStore;

	Notebook notebook;
	notebook.name    = L"old";
	notebook.isDirty = true;
	notebook.usn     = 0;

	noteStore.createdNotebooks.push_back(notebook);

	notebook.name    = L"new";
	notebook.isDirty = false;
	notebook.usn     = 2;

	notebookProcessor.Update(notebook, noteStore);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name,    L"new");
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).isDirty, false);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).usn,     2);
}
