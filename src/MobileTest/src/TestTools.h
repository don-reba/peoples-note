#pragma once

#include "resourceppc.h"
#include "Tools.h"

void TestToolsLoadStringResource(Test & test)
{
	std::wstring charString = Tools::LoadStringResource(IDS_CHAR_STRING);
	TEST_CHECK_EQUAL(charString.length(), 1);
	TEST_CHECK_EQUAL(charString, L".");

	std::wstring testString = Tools::LoadStringResource(IDS_TEST_STRING);
	TEST_CHECK_EQUAL(testString.length(), 4);
	TEST_CHECK_EQUAL(testString, L"Test");
}

void TestToolsLoadHtmlResource(Test & test)
{
	std::string html = "<html></html>";
	HtmlResource resource = Tools::LoadHtmlResource(IDH_TEST);
	TEST_CHECK_EQUAL(memcmp(resource.data, html.c_str(), html.length()), 0);
	TEST_CHECK_EQUAL(resource.size, html.size());
}