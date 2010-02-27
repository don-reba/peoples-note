#include "stdafx.h"

#include "Test.h"

#include "TestDataStore.h"
#include "TestTools.h"

#include <fstream>

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

	TestDataStoreLoad(test);
	TestToolsLoadHtmlResource(test);
	TestToolsLoadStringResource(test);

	test.Report();

	OpenFile(resultsFileName);

	return 0;
}
