#pragma once

class IUserModel;

class Transaction
{
private:

	IUserModel & userModel;

	// thread-local counter index
	static DWORD counterIndex;

public:

	Transaction(IUserModel & userModel);
	~Transaction();

private:

	static int GetCount();

	static void SetCount(int count);
};
