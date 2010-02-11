#include "stdafx.h"
#include "Notebook.h"

BOOST_AUTO_TEST_CASE(Notebook_Empty_Test)
{
	Notebook notebook;
	BOOST_CHECK_EQUAL(notebook.GetName(), L"");
	BOOST_CHECK_EQUAL(notebook.GetNotes().size(), 0);
}

BOOST_AUTO_TEST_CASE(Notebook_Name_Test)
{
	Notebook notebook;

	notebook.SetName(L"");
	BOOST_CHECK_EQUAL(notebook.GetName(), L"");

	notebook.SetName(L"test");
	BOOST_CHECK_EQUAL(notebook.GetName(), L"test");
}
