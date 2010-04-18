#include "stdafx.h"
#include "Transaction.h"
#include "IUserModel.h"

DWORD Transaction::counterIndex = ::TlsAlloc();

Transaction::Transaction(IUserModel & userModel)
	: userModel (userModel)
{
	int count(GetCount());
	if (count == 0)
		userModel.BeginTransaction();
	SetCount(count + 1);
}

Transaction::~Transaction()
{
	int count(GetCount());
	if (count == 1)
		userModel.EndTransaction();
	SetCount(count - 1);
}

int Transaction::GetCount()
{
	return reinterpret_cast<int>(::TlsGetValue(counterIndex));
}

void Transaction::SetCount(int count)
{
	::TlsSetValue(counterIndex, reinterpret_cast<void*>(count));
}
