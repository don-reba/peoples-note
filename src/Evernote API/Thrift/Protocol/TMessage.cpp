#include "stdafx.h"
#include "TMessage.h"

using namespace Thrift;
using namespace Thrift::Protocol;

TMessage::TMessage()
	: type  (MessageUnknown)
	, seqID (0)
{
}

TMessage::TMessage
	( const wchar_t      * name
	,       TMessageType   type
	,       int            seqID)
	: name  (name)
	, type  (type)
	, seqID (seqID)
{
}

const wchar_t * TMessage::GetName() const
{
	return name.c_str();
}

TMessageType TMessage::GetType() const
{
	return type;
}

int TMessage::GetSeqID() const
{
	return seqID;
}

void TMessage::SetName(const wchar_t * value)
{
	name = value;
}

void TMessage::SetType(TMessageType value)
{
	type = value;
}

void TMessage::SetSeqID(int value)
{
	seqID = value;
}
