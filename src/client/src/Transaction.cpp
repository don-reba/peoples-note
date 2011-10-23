#include "stdafx.h"
#include "Transaction.h"
#include "IUserModel.h"

LONG_PTR Transaction::counter(::TlsAlloc());

Transaction::Transaction(IUserModel & userModel)
	: userModel (userModel)
{
	LONG_PTR count(GetCount());
	if (count == 0)
		userModel.BeginTransaction();
	SetCount(count + 1);
}

Transaction::~Transaction()
{
	LONG_PTR count(GetCount());
	if (count == 1)
		userModel.EndTransaction();
	SetCount(count - 1);
}

LONG_PTR Transaction::GetCount()
{
	return reinterpret_cast<LONG_PTR>(::TlsGetValue(counter));
}

void Transaction::SetCount(LONG_PTR count)
{
	::TlsSetValue(counter, reinterpret_cast<LPVOID>(count));
}
