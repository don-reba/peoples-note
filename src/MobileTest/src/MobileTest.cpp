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

template<std::size_t N>
bool Contains(const wchar_t * (&list)[N], const wchar_t * query)
{
	for (int i(0); i != N; ++i)
	{
		if (0 == wcscmp(list[i], query))
			return true;
	}
	return false;
}

int WINAPI WinMain
	( HINSTANCE instance
	, HINSTANCE previousInstance
	, LPTSTR    cmdLine
	, int       cmdShow
	)
{
	SHInitExtraControls();
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	const wchar_t * resultsFileName(L"results.txt");
	std::wofstream resultsFile(resultsFileName);

	const wchar_t * exclusionList[] =
		{ L".\\src\\TestUserModelTransactions.cpp"
		};

	TEST_SUITE test(resultsFile);
	foreach (TEST_BASE * test_case, TEST_LIST)
	{
		if (Contains(exclusionList, test_case->file))
			continue;
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
