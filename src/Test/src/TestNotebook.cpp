#include "stdafx.h"
#include "Notebook.h"

using namespace std;

BOOST_AUTO_TEST_CASE(Notebook_Name_Test)
{
	Guid guid("test-guid");

	Notebook notebook(guid, L"notebook");

	BOOST_CHECK_EQUAL(static_cast<string>(notebook.GetGuid()), "test-guid");
	BOOST_CHECK_EQUAL(notebook.GetName(), L"notebook");
}
