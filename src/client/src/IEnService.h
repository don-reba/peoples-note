#pragma once

#include "AuthenticationToken.h"

class IUserStore;
class INoteStore;

class IEnService
{
public:

	typedef boost::shared_ptr<IUserStore> UserStore;
	typedef boost::shared_ptr<INoteStore> NoteStore;

public:

	virtual UserStore GetUserStore() = 0;

	virtual NoteStore GetNoteStore
		( const AuthenticationToken & token
		) = 0;
};
