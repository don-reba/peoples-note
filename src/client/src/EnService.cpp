#include "stdafx.h"
#include "EnService.h"

#include "ILogger.h"
#include "NoteStore.h"

using namespace boost;
using namespace std;

EnService::EnService(ILogger & logger)
	: logger (logger)
{
}

IEnService::NoteStorePtr EnService::GetNoteStore
	( const AuthenticationToken & token
	, const wstring             & shardId
	)
{
	return make_shared<NoteStore>(token, shardId);
}
