#pragma once

#include "../Protocol/TBinary.h"

namespace Thrift
{
namespace Transport
{

	class TTransport
	{
	public:

		virtual bool IsOpen() = 0;

		virtual void Open() = 0;

		virtual void Close() = 0;

		virtual int Read
			( Protocol::TBinary & buf
			, int                 off
			, int                 len
			) = 0;

		virtual void Write
			( const Protocol::TBinary & buf
			,       int                 off
			,       int                 len
			) = 0;

		virtual void Flush() = 0;

		bool Peek();

		int ReadAll
			( Protocol::TBinary & buf
			, int                 off
			, int                 len
			);
	};

} // Transport
} // Thrift
