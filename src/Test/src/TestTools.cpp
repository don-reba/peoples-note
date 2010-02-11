#include "stdafx.h"
#include "Tools.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(ToolsConvertToAnsi_Test1)
{
	string testString = Tools::ConvertToAnsi(L"Test");
	BOOST_REQUIRE_EQUAL(testString, "Test");
}

BOOST_AUTO_TEST_CASE(ToolsConvertToAnsi_Test2)
{
	string emptyString = Tools::ConvertToAnsi(L"");
	BOOST_REQUIRE_EQUAL(emptyString, "");
}

BOOST_AUTO_TEST_CASE(ToolsConvertToUtf8_Test1)
{
	vector<unsigned char> testString = Tools::ConvertToUtf8(L"Test");
	BOOST_REQUIRE_EQUAL(testString.size(), 4);
	BOOST_CHECK_EQUAL(testString.at(0), 'T');
	BOOST_CHECK_EQUAL(testString.at(1), 'e');
	BOOST_CHECK_EQUAL(testString.at(2), 's');
	BOOST_CHECK_EQUAL(testString.at(3), 't');
}

BOOST_AUTO_TEST_CASE(ToolsConvertToUtf8_Test2)
{
	vector<unsigned char> emptyString = Tools::ConvertToUtf8(L"");
	BOOST_REQUIRE_EQUAL(emptyString.size(), 0);
}

BOOST_AUTO_TEST_CASE(ToolsConvertToUnicode_Test1)
{
	wstring testString = L"Test";
	BOOST_REQUIRE_EQUAL(testString, L"Test");
}

BOOST_AUTO_TEST_CASE(ToolsConvertToUnicode_Test2)
{
	wstring emptyString = Tools::ConvertToUnicode("");
	BOOST_REQUIRE_EQUAL(emptyString, L"");
}
