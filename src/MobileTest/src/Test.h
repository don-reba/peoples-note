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
	std::wstring currentTestName;
	bool needHeader;

public:

	TEST_SUITE(std::wostream & out)
		: passCount_ (0)
		, failCount_ (0)
		, needHeader (false)
		, out_       (out)
	{
	}

	template <typename T>
	void Check(T e, LPCWSTR str)
	{
		if (RecordResult(e))
			out_ << L"!(" << str << L")\n";
	}

	template <typename T, typename P>
	void CheckException(const T & e, P p, LPCWSTR eStr, LPCWSTR pStr)
	{
		if (RecordResult(p(e)))
			out_ << L"!" << pStr << L"(" << eStr << L")\n";
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

	void Pass()
	{
		++passCount_;
	}

	void Fail()
	{
		PrintHeader();
		++failCount_;
	}

	void HandleException(const std::exception & e)
	{
		PrintHeader();
		using namespace Tools;
		out_ << L"exception: " << ConvertToUnicode(e.what()) << L'\n';
	}

	void HandleException()
	{
		PrintHeader();
		using namespace Tools;
		out_ << L"exception\n";
	}

	void PrintReport()
	{
		out_ << passCount_ << L"/" << (failCount_ + passCount_) << L" tests passed\n";
	}

	void SetTest(const wchar_t * name)
	{
		currentTestName = name;
		needHeader = true;
	}

	void UnexpectedException(LPCWSTR eStr)
	{
		Fail();
		out_ << "unexpected exception; expected " << eStr << "\n";
	}

private:

	void PrintHeader()
	{
		if (!needHeader)
			return;
		out_ << currentTestName << L'\n';
		needHeader = false;
	}

	bool RecordResult(bool result)
	{
		result ? Pass() : Fail();
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
	const wchar_t * name;
	TEST_BASE() { TEST_LIST.push_back(this); }
	virtual void Run(TEST_SUITE&) = 0;
};

#define FIXTURE_TEST_CASE(test_name, F)              \
    struct test_name : F                             \
    {                                                \
        void Run(TEST_SUITE&);                       \
    };                                               \
    struct test_name##_runner : TEST_BASE            \
    {                                                \
        test_name##_runner() { name = L#test_name; } \
        void Run(TEST_SUITE & TEST)                  \
        {                                            \
            test_name().Run(TEST);                   \
        }                                            \
    };                                               \
    static test_name##_runner test_name##instance;   \
    void test_name::Run(TEST_SUITE & TEST)           \

struct EMPTY_FIXTURE {};

#define AUTO_TEST_CASE(test_name)               \
    FIXTURE_TEST_CASE(test_name, EMPTY_FIXTURE) \

//------
// Tools
//------

class MESSAGE_EQUALS
{
private:

	std::string message;

public:

	MESSAGE_EQUALS(std::string message) : message (message) {}

	bool operator () (const std::exception & e)
	{
		return message == e.what();
	}
};

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
        TEST.Fail();                 \
        TEST.HandleException(e);     \
    }                                \
    catch (...)                      \
    {                                \
        TEST.Fail();                 \
        TEST.HandleException();      \
    }                                \

#define TEST_CHECK(expr)                           \
    TEST_OPEN TEST.Check(expr, L#expr); TEST_CLOSE \

#define TEST_CHECK_EQUAL(l, r)                            \
    TEST_OPEN TEST.CheckEqual(l, r, L#l, L#r); TEST_CLOSE \

#define TEST_CHECK_NOT_EQUAL(l, r)                           \
    TEST_OPEN TEST.CheckNotEqual(l, r, L#l, L#r); TEST_CLOSE \

#define TEST_CHECK_EXCEPTION(expr, exception, predicate)                   \
    try                                                              \
    {                                                                \
        expr;                                                        \
    }                                                                \
    catch (const exception & e)                                      \
    {                                                                \
        TEST_OPEN                                                    \
        TEST.CheckException(e, predicate, L#exception, L#predicate); \
        TEST_CLOSE                                                   \
    }                                                                \
    catch (...)                                                      \
    {                                                                \
        TEST.UnexpectedException(L#exception);                       \
    }                                                                \

