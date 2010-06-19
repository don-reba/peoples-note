#include "stdafx.h"
#include "EnService.h"

#include "NoteStore.h"
#include "UserStore.h"

using namespace boost;
using namespace std;

IEnService::UserStorePtr EnService::GetUserStore()
{
	return make_shared<UserStore>();
}

IEnService::NoteStorePtr EnService::GetNoteStore
	( const AuthenticationToken & token
	, const wstring             & shardId
	)
{
	return make_shared<NoteStore>(token, shardId);
}
