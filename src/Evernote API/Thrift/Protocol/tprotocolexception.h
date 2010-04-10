#pragma once

#include "../TException.h"

namespace Thrift
{
namespace Protocol
{

class TProtocolException : public TException
{
public:

	enum ExceptionType
	{
		Unknown      = 0,
		InvalidData  = 1,
		NegativeSize = 2,
		SizeLimit    = 3,
		BadVersion   = 4,
	};

	private:

		ExceptionType type;

	public:

		TProtocolException(const wchar_t * message);

		TProtocolException(ExceptionType type, const wchar_t * message);

		ExceptionType GetType();
};

} // Protocol
} // Thrift
