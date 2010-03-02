#include "stdafx.h"

#include "Test.h"

#include <fstream>
#include <vector>

std::vector<TEST_BASE*> TEST_LIST;

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

	TEST_SUITE test(resultsFile);
	foreach (TEST_BASE * test_case, TEST_LIST)
	{
		test.SetTest(test_case->name);
		try
		{
			test_case->Run(test);
		}
		catch (const std::exception & e)
		{
			test.HandleException(e);
		}
		catch (...)
		{
			test.HandleException();
		}
	}
	test.PrintReport();

	OpenFile(resultsFileName);

	return 0;
}
