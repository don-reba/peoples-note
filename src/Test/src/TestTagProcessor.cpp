#include "stdafx.h"
#if 0
#include "TagProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Tag.h"

#include <algorithm>

using namespace std;

BOOST_AUTO_TEST_CASE(TagProcessor_Add_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag;
	tag.name = L"test";

	noteProcessor.Add(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.tags.at(0).name, L"test");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Delete_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag;
	tag.name = L"test";

	noteProcessor.Delete(tag);

	BOOST_CHECK_EQUAL(userModel.deletedTags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedTags.at(0).name, L"test");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Rename_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

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

	noteProcessor.Rename(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 5);
	BOOST_CHECK_EQUAL(userModel.tags.at(4).name, L"test(4)");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Upload_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag;
	tag.name = L"test";

	noteProcessor.Upload(tag);

	BOOST_CHECK_EQUAL(noteStore.createdTags.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdTags.at(0).name, L"test");
}
#endif // 0