#include "stdafx.h"
#include "TStreamTransport.h"

using namespace std;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

TStreamTransport::TStreamTransport
	( std::istream & istream
	, std::ostream & ostream
	)
	: istream (istream)
	, ostream (ostream)
{
}

bool TStreamTransport::IsOpen()
{
	return !istream && !ostream;
}

void TStreamTransport::Open()
{
}

void TStreamTransport::Close()
{
}

int TStreamTransport::Read(TBinary & buf, int off, int len)
{
	istream::pos_type begin(istream.tellg());
	istream.read(reinterpret_cast<char*>(&buf.at(off)), len);
	istream::pos_type end(istream.tellg());
	return end - begin;
}

void TStreamTransport::Write(const TBinary & buf, int off, int len)
{
	ostream.write(reinterpret_cast<const char*>(&buf.at(off)), len);
}

void TStreamTransport::Flush()
{
	ostream.flush();
}
