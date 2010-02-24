#include "stdafx.h"
#include "Notebook.h"

BOOST_AUTO_TEST_CASE(Notebook_Name_Test)
{
	Notebook notebook(L"notebook");

	BOOST_CHECK_EQUAL(notebook.GetName(), L"notebook");
}
