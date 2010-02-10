#include "stdafx.h"
#include "Tools.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(ToolsConvertToUnicode_Test)
{
	wstring testString = L"Test";
	BOOST_CHECK_EQUAL(testString, L"Test");

	wstring emptyString = Tools::ConvertToUnicode("");
	BOOST_CHECK_EQUAL(emptyString, L"");
}

BOOST_AUTO_TEST_CASE(ToolsConvertToAnsi_Test)
{
	string testString = Tools::ConvertToAnsi(L"Test");
	BOOST_CHECK_EQUAL(testString, "Test");

	string emptyString = Tools::ConvertToAnsi(L"");
	BOOST_CHECK_EQUAL(emptyString, "");
}
