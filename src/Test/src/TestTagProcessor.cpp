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

	tagProcessor.Add(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.tags.at(0).name, L"test");
}

BOOST_FIXTURE_TEST_CASE(TagProcessor_Delete_Test, TagProcessorFixture)
{
	Tag tag;

	tagProcessor.Delete(tag);

	BOOST_CHECK_EQUAL(userModel.expungedTags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.expungedTags.at(0), tag.guid);
}

BOOST_FIXTURE_TEST_CASE(TagProcessor_Rename_Test, TagProcessorFixture)
{
	userModel.tags.push_back(Tag());
	userModel.tags.back().name = L"test";
	userModel.tags.push_back(Tag());
	userModel.tags.back().name = L"test(1)";
	userModel.tags.push_back(Tag());
	userModel.tags.back().name = L"test(2)";
	userModel.tags.push_back(Tag());
	userModel.tags.back().name = L"test(3)";

	Tag tag;
	tag.name = L"test";

	tagProcessor.RenameAdd(userModel.tags.at(0), tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 5);
	BOOST_CHECK_EQUAL(userModel.tags.at(0).name, L"test(4)");
	BOOST_CHECK_EQUAL(userModel.tags.at(4).name, L"test");
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
