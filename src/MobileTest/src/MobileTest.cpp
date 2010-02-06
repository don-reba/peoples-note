#include "stdafx.h"

#include "resourceppc.h"
#include "Test.h"
#include "Tools.h"

#include <fstream>


void TestToolsLoadStringResource(Test & test)
{
	std::wstring charString = Tools::LoadStringResource(IDS_CHAR_STRING);
	TEST_CHECK_EQUAL(test, charString.length(), 1);
	TEST_CHECK_EQUAL(test, charString, L".");

	std::wstring testString = Tools::LoadStringResource(IDS_TEST_STRING);
	TEST_CHECK_EQUAL(test, testString.length(), 4);
	TEST_CHECK_EQUAL(test, testString, L"Test");
}

void TestToolsLoadHtmlResource(Test & test)
{
	std::string html = "<html></html>";
	HtmlResource resource = Tools::LoadHtmlResource(IDH_TEST);
	TEST_CHECK_EQUAL(test, memcmp(resource.data, html.c_str(), html.length()), 0);
	TEST_CHECK_EQUAL(test, resource.size, html.size());
}

void OpenFile(const wchar_t * file)
{
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.fMask  = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"Open";
	sei.lpFile = file;
	ShellExecuteEx(&sei);
}

int WINAPI WinMain
	( HINSTANCE instance
	, HINSTANCE previousInstance
	, LPTSTR    cmdLine
	, int       cmdShow
	)
{
	const wchar_t * resultsFileName(L"results.txt");
	std::wofstream resultsFile(resultsFileName);

	Test test(resultsFile);

	TestToolsLoadHtmlResource(test);
	TestToolsLoadStringResource(test);

	test.Report();

	OpenFile(resultsFileName);

	return 0;
}
