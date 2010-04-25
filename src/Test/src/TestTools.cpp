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

BOOST_AUTO_TEST_CASE(ToolsDecodeBase64)
{
	const int count(6);

	vector<const wchar_t *> in  (count);
	vector<Blob>            out (count);

	in.at(0) = L"";

	in.at(1) = L"Ag==";
	out.at(1).push_back(2);

	in.at(2) = L"AgM=";
	out.at(2).push_back(2);
	out.at(2).push_back(3);

	in.at(3) = L"AgMF";
	out.at(3).push_back(2);
	out.at(3).push_back(3);
	out.at(3).push_back(5);

	in.at(4) = L"AgMF\r\nBw==";
	out.at(4).push_back(2);
	out.at(4).push_back(3);
	out.at(4).push_back(5);
	out.at(4).push_back(7);

	in.at(5) = L"Ag==\r\n";
	out.at(5).push_back(2);

	for (int i(0); i != count; ++i)
	{
		Blob result;
		Tools::DecodeBase64(in.at(i), result);
		BOOST_CHECK_EQUAL(result.size(), out.at(i).size());
		if (result.size() == out.size())
		{
			for (int j(0); j != count; ++j)
				BOOST_CHECK_EQUAL(result.at(j), out.at(i).at(j));
		}
	}
}

BOOST_AUTO_TEST_CASE(ToolsStartsWith)
{
	BOOST_CHECK(!Tools::StartsWith(NULL, L""));
	BOOST_CHECK(!Tools::StartsWith(NULL, NULL));
	BOOST_CHECK(!Tools::StartsWith(L"",  NULL));

	BOOST_CHECK(Tools::StartsWith(L"",    L""));
	BOOST_CHECK(Tools::StartsWith(L"a",   L""));
	BOOST_CHECK(Tools::StartsWith(L"abc", L"ab"));

	BOOST_CHECK(!Tools::StartsWith(L"",    L"a"));
	BOOST_CHECK(!Tools::StartsWith(L"a",   L"b"));
	BOOST_CHECK(!Tools::StartsWith(L"abc", L"bc"));
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
