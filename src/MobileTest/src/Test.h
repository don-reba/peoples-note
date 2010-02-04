#include <exception>
#include <iostream>
#include <string>

#include "Tools.h"


#define TEST_CHECK_EQUAL(tTest, l, r) \
	try { test.CheckEqual(l, r, L#l, L#r); } catch (std::exception e) { test.HandleException(e); }


class Test
{
private:

	int passCount_;
	int failCount_;
	std::wostream & out_;

public:

	Test(std::wostream & out)
		: passCount_ (0)
		, failCount_ (0)
		, out_       (out)
	{
	}

	template<typename T1, typename T2>
	void CheckEqual(T1 v1, T2 v2, LPCWSTR v1Str, LPCWSTR v2Str)
	{
		const wchar_t * result(NULL);
		if (v1 == v2)
		{
			++passCount_;
		}
		else
		{
			out_ << v1Str << L" == " << v2Str << L" [" << v1 << L"!=" << v2 << L"]\n";
			++failCount_;
		}
	}

	void HandleException(const std::exception & e)
	{
		out_ << L"exception: " << Tools::ConvertToUnicode(e.what()) << L"\n";
		++failCount_;
	}

	void Report()
	{
		out_ << passCount_ << L"/" << (failCount_ + passCount_) << L" tests passed\n";
	}
};
