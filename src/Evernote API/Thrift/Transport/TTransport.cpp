#include "stdafx.h"
#include "TTransport.h"

#include "TTransportException.h"

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

bool TTransport::Peek()
{
	return IsOpen();
}

int TTransport::ReadAll
	( TBinary & buf
	, int       off
	, int       len
	)
{
	int got = 0;
	int ret = 0;

	while (got < len)
	{
		ret = Read(buf, off + got, len - got);
		if (ret <= 0)
			throw TTransportException(L"Cannot read, Remote side has closed");
		got += ret;
	}

	return got;
}
