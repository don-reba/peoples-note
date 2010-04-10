#include "stdafx.h"
#include "TApplicationException.h"

#include "Protocol/TProtocolUtil.h"

using namespace std;
using namespace Thrift;
using namespace Thrift::Protocol;

TApplicationException::TApplicationException(const wchar_t * message)
	: type       (Unknown)
	, TException (message)
{
}

TApplicationException::TApplicationException(ExceptionType type, const wchar_t * message)
	: type       (type)
	, TException (message)
{
}

TApplicationException::ExceptionType TApplicationException::GetType()
{
	return type;
}

TApplicationException TApplicationException::Read(TProtocol & iprot)
{
	ExceptionType type;
	TString       message;
	for (;;)
	{
		TField field;
		iprot.ReadFieldBegin(field);
		if (field.GetType() == TypeStop)
			break;
		switch (field.GetID())
		{
		case 1:
			if (field.GetType() == TypeString)
				iprot.ReadString(message);
			else
				TProtocolUtil::Skip(iprot, field.GetType());
			break;
		case 2:
			if (field.GetType() == TypeI32)
				type = static_cast<ExceptionType>(iprot.ReadI32());
			else
				TProtocolUtil::Skip(iprot, field.GetType());
			break;
		}
	}
	iprot.ReadStructEnd();
	return TApplicationException(type, message.c_str());
}

void TApplicationException::Write(TProtocol & oprot) const
{
	TStruct struc;
	struc.SetName(L"TApplicationException");
	oprot.WriteStructBegin(struc);

	const wchar_t * message = GetMessage();
	if (message && *message)
	{
		TField field;
		field.SetName(L"message");
		field.SetType(TypeString);
		field.SetID(1);
		oprot.WriteFieldBegin(field);
		oprot.WriteString(TString(message));
		oprot.WriteFieldEnd();
	}

	TField field;
	field.SetName(L"type");
	field.SetType(TypeI32);
	field.SetID(2);
	oprot.WriteFieldBegin(field);
	oprot.WriteI32(type);
	oprot.WriteFieldEnd();
	oprot.WriteFieldStop();
	oprot.WriteStructEnd();
}
