#include "stdafx.h"
#include "UserUpdater.h"

#include "Guid.h"
#include "INoteStore.h"
#include "IUserModel.h"

UserUpdater::UserUpdater
	( INoteStore & noteStore
	, IUserModel & userModel
	)
	: noteStore (noteStore)
	, userModel (userModel)
{
}

void UserUpdater::UpdateNote(Guid & note)
{
}

void UserUpdater::UpdateNotebook(Guid & notebook)
{
}

void UserUpdater::UpdateTag(Guid & tag)
{
}
