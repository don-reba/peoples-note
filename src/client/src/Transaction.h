#pragma once

class IUserModel;

class Transaction
{
private:

	IUserModel & userModel;

public:

	Transaction(IUserModel & userModel);
	~Transaction();
};
