#include "stdafx.h"
#include "PhotoEditorModel.h"

#include "MockRegistryKey.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct PhotoEditorModelFixture
{
	MockRegistryKey registryKey;

	PhotoEditorModel photoEditorModel;

	PhotoEditorModelFixture()
		: photoEditorModel (registryKey)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorModel_GetQuality
	, PhotoEditorModelFixture
	)
{
	registryKey.data[L"photo quality"] = L"test-quality";

	wstring quality;

	photoEditorModel.GetQuality(quality);

	BOOST_CHECK_EQUAL(quality, L"test-quality");
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorModel_GetResolution
	, PhotoEditorModelFixture
	)
{
	registryKey.data[L"photo resolution"] = L"test-resolution";

	wstring resolution;

	photoEditorModel.GetResolution(resolution);

	BOOST_CHECK_EQUAL(resolution, L"test-resolution");
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorModel_SetQuality
	, PhotoEditorModelFixture
	)
{
	photoEditorModel.SetQuality(L"test-quality");

	BOOST_CHECK_EQUAL(registryKey.data[L"photo quality"], L"test-quality");
}

BOOST_FIXTURE_TEST_CASE
	( PhotoEditorModel_SetResolution
	, PhotoEditorModelFixture
	)
{
	photoEditorModel.SetResolution(L"test-resolution");

	BOOST_CHECK_EQUAL(registryKey.data[L"photo resolution"], L"test-resolution");
}
