#pragma once

#include "Protocol/TProtocol.h"
#include "TException.h"

namespace Thrift
{

	class TApplicationException : public TException
	{
	public:

		enum ExceptionType
		{
			Unknown            = 0,
			UnknownMethod      = 1,
			InvalidMessageType = 2,
			WrongMethodName    = 3,
			BadSequenceID      = 4,
			MissingResult      = 5,
		};

	private:

		ExceptionType type;

	public:

		TApplicationException(const wchar_t * message);

		TApplicationException(ExceptionType type, const wchar_t * message);

		ExceptionType GetType();

		static TApplicationException Read(Protocol::TProtocol & iprot);

		void Write(Protocol::TProtocol & oprot) const;
	};

} // Thrift
