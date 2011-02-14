#pragma once

#include "AuthenticationToken.h"

class ILogger;
class INoteStore;
class IUserStore;

class IEnService
{
public:

	typedef boost::shared_ptr<IUserStore> UserStorePtr;
	typedef boost::shared_ptr<INoteStore> NoteStorePtr;

public:

	virtual UserStorePtr GetUserStore() = 0;

	virtual NoteStorePtr GetNoteStore
		( const AuthenticationToken & token
		, const std::wstring        & shardId
		) = 0;
};
