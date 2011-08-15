#pragma once

class Guid;
class IUserModel;
class INoteStore;

class UserUpdater
{
private:

	IUserModel & userModel;
	INoteStore & noteStore;

public:

	UserUpdater
		( INoteStore & noteStore
		, IUserModel & userModel
		);

	void UpdateNote(Guid & note);

	void UpdateNotebook(Guid & notebook);

	void UpdateTag(Guid & tag);
};
