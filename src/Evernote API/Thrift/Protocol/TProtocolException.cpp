#include "stdafx.h"
#include "TProtocolException.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TProtocolException::TProtocolException(const wchar_t * message)
	: type       (Unknown)
	, TException (message)
{
}

TProtocolException::TProtocolException(ExceptionType type, const wchar_t * message)
	: type       (type)
	, TException (message)
{
}

TProtocolException::ExceptionType TProtocolException::GetType()
{
	return type;
}
