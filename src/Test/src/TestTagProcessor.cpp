#include "stdafx.h"
#include "TagProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Tag.h"

#include <algorithm>

using namespace std;

struct TagProcessorFixture
{
	MockUserModel userModel;

	TagProcessor tagProcessor;

	TagProcessorFixture()
		: tagProcessor (userModel)
	{
	}
};

BOOST_FIXTURE_TEST_CASE(TagProcessor_Add_Test, TagProcessorFixture)
{
	Tag tag;
	tag.name = L"test";

	tagProcessor.AddLocal(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.tags.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(TagProcessor_DeleteLocal_Test, TagProcessorFixture)
{
	Tag tag;

	tagProcessor.DeleteLocal(tag);

	BOOST_CHECK_EQUAL(userModel.expungedTags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.expungedTags.at(0), tag.guid);
}

//BOOST_FIXTURE_TEST_CASE(TagProcessor_Upload_Test, TagProcessorFixture)
//{
//	MockNoteStore noteStore;
//	
//	Tag tag;
//	tag.name = L"test";
//
//	tagProcessor.Upload(tag, noteStore);
//
//	BOOST_CHECK_EQUAL(noteStore.createdTags.size(), 1);
//	BOOST_CHECK_EQUAL(noteStore.createdTags.at(0).name, L"test");
//}
