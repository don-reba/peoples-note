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

void UserUpdater::UpdateNote(const Guid & guid)
{
	Note note;
	noteStore.GetNote(guid, note);
	userModel.UpdateNote(guid, note);
}

void UserUpdater::UpdateNotebook(const Guid & guid)
{
	Notebook notebook;
	noteStore.GetNotebook(guid, notebook);
	userModel.UpdateNotebook(guid, notebook);
}

void UserUpdater::UpdateTag(const Guid & guid)
{
	Tag tag;
	noteStore.GetTag(guid, tag);
	userModel.UpdateTag(guid, tag);
}
