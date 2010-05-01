#include "stdafx.h"
#include "MockEnService.h"

#include "IUserModel.h"

using namespace boost;
using namespace std;

MockEnService::MockEnService()
	: userStore (make_shared<MockUserStore>())
	, noteStore (make_shared<MockNoteStore>())
{
}

IEnService::UserStore MockEnService::GetUserStore()
{
	return userStore;
}

IEnService::NoteStore MockEnService::GetNoteStore()
{
	return noteStore;
}
