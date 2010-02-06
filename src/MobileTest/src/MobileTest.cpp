#include "stdafx.h"

#include "resourceppc.h"
#include "Test.h"
#include "Tools.h"

#include <fstream>
#include <string>


void TestToolsLoadStringResource(Test & test, HINSTANCE instance)
{
	std::wstring charString = Tools::LoadStringResource(IDS_CHAR_STRING);
	TEST_CHECK_EQUAL(test, charString.length(), 1);
	TEST_CHECK_EQUAL(test, charString, L".");

	std::wstring testString = Tools::LoadStringResource(IDS_TEST_STRING);
	TEST_CHECK_EQUAL(test, testString.length(), 4);
	TEST_CHECK_EQUAL(test, testString, L"Test");
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

	TestToolsLoadStringResource(test, instance);
	test.Report();

	OpenFile(resultsFileName);

	return 0;
}
