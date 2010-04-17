#include "stdafx.h"
#include "Transaction.h"
#include "IUserModel.h"

Transaction::Transaction(IUserModel & userModel)
	: userModel (userModel)
{
	userModel.BeginTransaction();
}

Transaction::~Transaction()
{
	userModel.EndTransaction();
}
