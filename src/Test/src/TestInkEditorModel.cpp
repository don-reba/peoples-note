#include "stdafx.h"
#include "InkEditorModel.h"

#include "MockRegistryKey.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct InkEditorModelFixture
{
	MockRegistryKey registryKey;

	InkEditorModel inkEditorModel;

	InkEditorModelFixture()
		: inkEditorModel (registryKey)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( InkEditorModel_GetPen
	, InkEditorModelFixture
	)
{
	wstring width;
	wstring color;

	inkEditorModel.GetPen(width, color);

	BOOST_CHECK_EQUAL(width, L"1px");
	BOOST_CHECK_EQUAL(color, L"black");

	registryKey.data[L"pen width"] = L"2px";
	registryKey.data[L"pen color"] = L"red";

	inkEditorModel.GetPen(width, color);

	BOOST_CHECK_EQUAL(width, L"2px");
	BOOST_CHECK_EQUAL(color, L"red");
}

BOOST_FIXTURE_TEST_CASE
	( InkEditorModel_SetPen
	, InkEditorModelFixture
	)
{
	inkEditorModel.SetPen(L"2px", L"red");

	BOOST_CHECK_EQUAL(registryKey.data[L"pen width"], L"2px");
	BOOST_CHECK_EQUAL(registryKey.data[L"pen color"], L"red");
}
