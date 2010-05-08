#pragma once

#include "AuthenticationToken.h"
#include "IEnService.h"
#include "MockNoteStore.h"
#include "MockUserStore.h"

class MockEnService : public IEnService
{
public:

	boost::shared_ptr<MockUserStore> userStore;
	boost::shared_ptr<MockNoteStore> noteStore;
	AuthenticationToken              authenticationToken;

public:

	MockEnService();

	virtual UserStorePtr GetUserStore();

	virtual NoteStorePtr GetNoteStore
		( const AuthenticationToken & token
		, const std::wstring        & shardId
		);
};
