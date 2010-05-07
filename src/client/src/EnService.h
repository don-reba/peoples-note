#pragma once

#include "IEnService.h"

class EnService : public IEnService
{
public:

	virtual UserStore GetUserStore();

	virtual NoteStore GetNoteStore
		( const AuthenticationToken & token
		);
};
