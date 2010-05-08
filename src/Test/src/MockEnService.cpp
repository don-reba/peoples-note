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

IEnService::UserStorePtr MockEnService::GetUserStore()
{
	return userStore;
}

IEnService::NoteStorePtr MockEnService::GetNoteStore
	( const AuthenticationToken & token
	, const wstring             & shardId
	)
{
	authenticationToken = token;
	return noteStore;
}
