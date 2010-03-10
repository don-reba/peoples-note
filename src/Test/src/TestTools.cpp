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
	vector<unsigned char> testChars;
	const char * test
		= reinterpret_cast<const char *>
		( Tools::ConvertToUtf8(L"Test", testChars)
		);
	BOOST_CHECK_EQUAL(::strcmp(test, "Test"), 0);
}

BOOST_AUTO_TEST_CASE(ToolsConvertToUtf8_Test2)
{
	vector<unsigned char> testChars;
	const unsigned char * test = Tools::ConvertToUtf8(L"", testChars);
	BOOST_REQUIRE_EQUAL(*test, 0);
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

BOOST_AUTO_TEST_CASE(ToolsUnixTimeToFileTime_Test1)
{
	time_t unixTime = 0;
	FILETIME fileTime;
	Tools::UnixTimeToFileTime(unixTime, fileTime);
	BOOST_CHECK_EQUAL(fileTime.dwLowDateTime,  0xD53E8000);
	BOOST_CHECK_EQUAL(fileTime.dwHighDateTime, 0x019DB1DE);
}

BOOST_AUTO_TEST_CASE(ToolsUnixTimeToFileTime_Test2)
{
	// values from:
	// http://blogs.msdn.com/oldnewthing/archive/2003/09/05/54806.aspx
	time_t unixTime = 0x3DE43B0C;
	FILETIME fileTime;
	Tools::UnixTimeToFileTime(unixTime, fileTime);
	BOOST_CHECK_EQUAL(fileTime.dwLowDateTime,  0x91150E00);
	BOOST_CHECK_EQUAL(fileTime.dwHighDateTime, 0x01C295C4);
}

BOOST_AUTO_TEST_CASE(ToolsUnixTimeToSystemTime_Test)
{
	time_t unixTime = 0;
	SYSTEMTIME systemTime;
	Tools::UnixTimeToSystemTime(unixTime, systemTime);
	BOOST_CHECK_EQUAL(systemTime.wYear,         1970);
	BOOST_CHECK_EQUAL(systemTime.wMonth,        1);
	BOOST_CHECK_EQUAL(systemTime.wDay,          1);
	BOOST_CHECK_EQUAL(systemTime.wHour,         0);
	BOOST_CHECK_EQUAL(systemTime.wMinute,       0);
	BOOST_CHECK_EQUAL(systemTime.wSecond,       0);
	BOOST_CHECK_EQUAL(systemTime.wMilliseconds, 0);
	BOOST_CHECK_EQUAL(systemTime.wDayOfWeek,    4);
}
