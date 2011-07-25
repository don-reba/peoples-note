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

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_AddLocal_Test, NotebookProcessorFixture)
{
	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.AddLocal(notebook);

	BOOST_CHECK_EQUAL(userModel.notebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notebooks.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_CreateRemote_Test, NotebookProcessorFixture)
{
	MockNoteStore noteStore;

	Notebook notebook;
	notebook.name = L"test";

	notebookProcessor.CreateRemote(notebook, noteStore);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_DeleteLocal_Test, NotebookProcessorFixture)
{
	Notebook notebook;

	notebookProcessor.DeleteLocal(notebook);

	BOOST_CHECK_EQUAL(userModel.expungedNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(userModel.expungedNotebooks.at(0), notebook.guid);
}

BOOST_FIXTURE_TEST_CASE(NotebookProcessor_UpdateRemote_Test, NotebookProcessorFixture)
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

	notebookProcessor.UpdateRemote(notebook, noteStore);

	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).name,    L"new");
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).isDirty, false);
	BOOST_CHECK_EQUAL(noteStore.createdNotebooks.at(0).usn,     2);
}
