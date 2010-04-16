#pragma once

class ILastUserModel;
class IUserModel;

class UserLoader
{
private:

	IUserModel     & userModel;
	ILastUserModel & lastUserModel;

public:

	UserLoader
		( IUserModel     & userModel
		, ILastUserModel & lastUserModel
		);

	void Run();

	void Save();
};
