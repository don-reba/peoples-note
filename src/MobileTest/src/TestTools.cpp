#include "stdafx.h"

#include "resourceppc.h"
#include "Test.h"
#include "Tools.h"

AUTO_TEST_CASE(TestToolsLoadStringResource)
{
	std::wstring charString = Tools::LoadStringResource(IDS_CHAR_STRING);
	TEST_CHECK_EQUAL(charString.length(), 1);
	TEST_CHECK_EQUAL(charString, L".");

	std::wstring testString = Tools::LoadStringResource(IDS_TEST_STRING);
	TEST_CHECK_EQUAL(testString.length(), 4);
	TEST_CHECK_EQUAL(testString, L"Test");
}

AUTO_TEST_CASE(TestToolsLoadHtmlResource)
{
	std::string html = "<html></html>";
	HtmlResource resource = Tools::LoadHtmlResource(L"test.htm");
	TEST_CHECK_EQUAL(memcmp(resource.data, html.c_str(), html.length()), 0);
	TEST_CHECK_EQUAL(resource.size, html.size());
}
