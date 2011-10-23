#pragma once

class IUserModel;

class Transaction
{
private:

	IUserModel & userModel;

	static LONG_PTR counter; // thread-local

public:

	Transaction(IUserModel & userModel);
	~Transaction();

private:

	static LONG_PTR GetCount();

	static void SetCount(LONG_PTR count);
};
