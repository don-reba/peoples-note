#pragma once

#include "IEnService.h"
#include "MockNoteStore.h"
#include "MockUserStore.h"

class MockEnService : public IEnService
{
public:

	boost::shared_ptr<MockUserStore> userStore;
	boost::shared_ptr<MockNoteStore> noteStore;

public:

	MockEnService();

	virtual UserStore GetUserStore();
	virtual NoteStore GetNoteStore();
};
