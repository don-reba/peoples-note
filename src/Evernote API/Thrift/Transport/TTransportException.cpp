#include "stdafx.h"
#include "TTransportException.h"

using namespace Thrift;
using namespace Thrift::Transport;

TTransportException::TTransportException(const wchar_t * message)
	: type       (Unknown)
	, TException (message)
{
}

TTransportException::TTransportException(ExceptionType type, const wchar_t * message)
	: type       (type)
	, TException (message)
{
}

TTransportException::ExceptionType TTransportException::GetType() const
{
	return type;
}
