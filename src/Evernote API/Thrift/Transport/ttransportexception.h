#pragma once

#include "../TException.h"

namespace Thrift
{
namespace Transport
{

	class TTransportException : public TException
	{
	public:

		enum ExceptionType
		{
			Unknown,
			NotOpen,
			AlreadyOpen,
			TimedOut,
			EndOfFile,
		};

	private:

		ExceptionType type;

	public:

		TTransportException(const wchar_t * message);

		TTransportException(ExceptionType type, const wchar_t * message);

		ExceptionType GetType() const;
	};

} // Transport
} // Thrift
