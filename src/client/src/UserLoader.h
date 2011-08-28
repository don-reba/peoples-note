#pragma once

class ICredentialsModel;
class ILastUserModel;

class UserLoader
{
private:

	ICredentialsModel & credentialsModel;
	ILastUserModel    & lastUserModel;

public:

	UserLoader
		( ICredentialsModel & credentialsModel
		, ILastUserModel    & lastUserModel
		);

	void Run();

private:

	void OnCommit();
};
