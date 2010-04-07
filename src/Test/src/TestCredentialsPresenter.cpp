#include "stdafx.h"
#include "CredentialsPresenter.h"
#include "MockCredentialsModel.h"
#include "MockCredentialsView.h"
#include "MockEnService.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(CredentialsPresenter_OkPass_Test)
{
	MockCredentialsModel credentialsModel;
	MockCredentialsView  credentialsView;
	MockEnService        enService;
	CredentialsPresenter presenter
		( credentialsModel
		, credentialsView
		, enService
		);

	credentialsModel.username = L"initial-usr";
	credentialsModel.password = L"initial-pwd";

	credentialsModel.SignalNeedsSet();
	BOOST_CHECK(credentialsView.isOpen);

	credentialsView.SignalCreated();

	BOOST_CHECK_EQUAL(credentialsView.username, L"initial-usr");
	BOOST_CHECK_EQUAL(credentialsView.password, L"initial-pwd");

	enService.credentialsValidity.IsGood  = true;
	enService.credentialsValidity.Message = L"test-msg";
	credentialsView.username = L"test-usr";
	credentialsView.password = L"test-pwd";
	credentialsView.SignalOk();

	BOOST_CHECK(!credentialsView.isOpen);
	BOOST_CHECK_EQUAL(credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"test-pwd");
}

BOOST_AUTO_TEST_CASE(CredentialsPresenter_OkFail_Test)
{
	MockCredentialsModel credentialsModel;
	MockCredentialsView  credentialsView;
	MockEnService        enService;
	CredentialsPresenter presenter
		( credentialsModel
		, credentialsView
		, enService
		);

	credentialsModel.SignalNeedsSet();
	credentialsView.SignalCreated();

	enService.credentialsValidity.IsGood  = false;
	enService.credentialsValidity.Message = L"test-msg";
	credentialsView.username = L"test-usr";
	credentialsView.password = L"test-pwd";
	credentialsView.SignalOk();

	BOOST_CHECK(credentialsView.isOpen);
	BOOST_CHECK_EQUAL(credentialsView.message,  L"test-msg");
	BOOST_CHECK_EQUAL(credentialsModel.username, L"");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"");
}

BOOST_AUTO_TEST_CASE(CredentialsPresenter_Cancel_Test)
{
	MockCredentialsModel credentialsModel;
	MockCredentialsView  credentialsView;
	MockEnService        enService;
	CredentialsPresenter presenter
		( credentialsModel
		, credentialsView
		, enService
		);

	credentialsModel.SignalNeedsSet();
	credentialsView.SignalCreated();

	credentialsView.username = L"test-usr";
	credentialsView.password = L"test-pwd";
	credentialsView.SignalCancel();

	BOOST_CHECK(!credentialsView.isOpen);
	BOOST_CHECK_EQUAL(credentialsModel.username, L"");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"");
}
