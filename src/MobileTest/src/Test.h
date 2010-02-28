#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "Tools.h"

class TEST_SUITE
{
private:

	int passCount_;
	int failCount_;
	std::wostream & out_;

public:

	TEST_SUITE(std::wostream & out)
		: passCount_ (0)
		, failCount_ (0)
		, out_       (out)
	{
	}

	template <typename T>
	void Check(T e, LPCWSTR str)
	{
		if (RecordResult(e))
			out_ << L"!(" << str << L")\n";
	}

	template <typename T1, typename T2>
	void CheckEqual(T1 v1, T2 v2, LPCWSTR v1Str, LPCWSTR v2Str)
	{
		if (RecordResult(v1 == v2))
			out_ << v1Str << L" == " << v2Str << L" [" << v1 << L" != " << v2 << L"]\n";
	}

	template <typename T1, typename T2>
	void CheckNotEqual(T1 v1, T2 v2, LPCWSTR v1Str, LPCWSTR v2Str)
	{
		if (RecordResult(v1 != v2))
			out_ << v1Str << L" != " << v2Str << L" [" << v1 << L" == " << v2 << L"]\n";
	}

	void Fail()
	{
		++failCount_;
	}

	void HandleException(const std::exception & e)
	{
		using namespace Tools;
		out_ << L"exception: " << ConvertToUnicode(e.what()) << L'\n';
	}

	void HandleException()
	{
		using namespace Tools;
		out_ << L"exception\n";
	}

	void Report()
	{
		out_ << passCount_ << L"/" << (failCount_ + passCount_) << L" tests passed\n";
	}

private:

	bool RecordResult(bool result)
	{
		if (result)
			++passCount_;
		else
			++failCount_;
		return !result;
	}
};

//----------
// Test case
//----------

struct TEST_BASE;

extern std::vector<TEST_BASE*> TEST_LIST;

struct TEST_BASE
{
	TEST_BASE() { TEST_LIST.push_back(this); }
	virtual std::string GetName() const = 0;
	virtual void Run(TEST_SUITE&) = 0;
};

#define AUTO_TEST_CASE(test_name)                          \
    struct test_name : TEST_BASE                           \
    {                                                      \
        void Run(TEST_SUITE&);                             \
		std::string GetName() const { return #test_name; } \
    };                                                     \
	static test_name test_name##instance;                  \
    void test_name::Run(TEST_SUITE & test)                 \

//------
// Tests
//------

#define TEST_OPEN \
    try           \
    {             \

#define TEST_CLOSE                   \
    }                                \
    catch (const std::exception & e) \
    {                                \
        test.Fail();                 \
        test.HandleException(e);     \
    }                                \
    catch (...)                      \
    {                                \
        test.Fail();                 \
        test.HandleException();      \
    }                                \

#define TEST_CHECK(expr)                           \
    TEST_OPEN test.Check(expr, L#expr); TEST_CLOSE \

#define TEST_CHECK_EQUAL(l, r)                            \
    TEST_OPEN test.CheckEqual(l, r, L#l, L#r); TEST_CLOSE \

#define TEST_CHECK_NOT_EQUAL(l, r)                           \
    TEST_OPEN test.CheckNotEqual(l, r, L#l, L#r); TEST_CLOSE \

