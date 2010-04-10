#pragma once

#include "../Protocol/TBinary.h"
#include "TTransport.h"

#include <iostream>

namespace Thrift
{
namespace Transport
{

	class TStreamTransport : public TTransport
	{
	private:

		std::istream & istream;
		std::ostream & ostream;

	public:

		TStreamTransport
			( std::istream & istream
			, std::ostream & ostream
			);

		virtual bool IsOpen();

		virtual void Open();

		virtual void Close();

		virtual int Read
			( Protocol::TBinary & buf
			, int                 off
			, int                 len
			);

		virtual void Write
			( const Protocol::TBinary & buf
			,       int                 off
			,       int                 len
			);

		virtual void Flush();
	};

} // Transport
} // Thrift
