#pragma once

#include "IEnService.h"

class EnService : public IEnService
{
public:

	virtual UserStorePtr GetUserStore();

	virtual NoteStorePtr GetNoteStore
		( const AuthenticationToken & token
		, const std::wstring        & shardId
		);
};
