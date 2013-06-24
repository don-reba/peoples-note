#pragma once

#include "IEnService.h"

class EnService : public IEnService
{
private:

	ILogger & logger;

public:

	EnService(ILogger & logger);

	virtual NoteStorePtr GetNoteStore
		( const AuthenticationToken & token
		, const std::wstring        & shardId
		);
};
