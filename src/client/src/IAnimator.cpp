#include "stdafx.h"
#include "IAnimator.h"

IAnimator::Connection::Connection()
	: animator(NULL)
{
}

IAnimator::Connection::Connection(IAnimator * animator, int id)
	: animator (animator)
	, id       (id)
{
}

void IAnimator::Connection::Disconnect()
{
	animator->Unsubscribe(id);
}
