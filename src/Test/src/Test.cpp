#include "stdafx.h"
#include "CredentialsPresenter.h"
#include "MockCredentialsModel.h"
#include "MockCredentialsView.h"
#include "Tools.h"

#include <iostream>
#include <string>

#define BOOST_TEST_MODULE const_string test
#include <boost/test/unit_test.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(std::wstring)

BOOST_AUTO_TEST_CASE(ToolsConvertToUnicodeTest)
{
	std::wstring testString = Tools::ConvertToUnicode("Test");
	BOOST_CHECK_EQUAL(testString.length(), 4);
	BOOST_CHECK_EQUAL(testString, L"Test");

	std::wstring emptyString = Tools::ConvertToUnicode("");
	BOOST_CHECK_EQUAL(emptyString.length(), 0);
	BOOST_CHECK_EQUAL(emptyString, L"");
}

BOOST_AUTO_TEST_CASE(CredentialsPresenterTest)
{
	MockCredentialsModel model;
	MockCredentialsView  view;
	CredentialsPresenter presenter(model, view);

	view.SetUsername(L"test-usr");
	view.SetPassword(L"test-pwd");
	view.SignIn();

	BOOST_CHECK_EQUAL(model.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(model.GetPassword(), L"test-pwd");
}
