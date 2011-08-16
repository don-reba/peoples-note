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

	void UpdateNote(const Guid & guid);

	void UpdateNotebook(const Guid & guid);

	void UpdateTag(const Guid & guid);
};
