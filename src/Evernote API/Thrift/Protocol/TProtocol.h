#pragma once

#include "TBinary.h"
#include "TField.h"
#include "TList.h"
#include "TMap.h"
#include "TMessage.h"
#include "TSet.h"
#include "TStruct.h"
#include "../Transport/TTransport.h"

namespace Thrift
{
namespace Protocol
{

class TProtocol
{
public:

	virtual Transport::TTransport & GetTransport() = 0;

	virtual void WriteMessageBegin (const TMessage & message) = 0;
	virtual void WriteMessageEnd   ()                         = 0;
	virtual void WriteStructBegin  (const TStruct & struc)    = 0;
	virtual void WriteStructEnd    ()                         = 0;
	virtual void WriteFieldBegin   (const TField & field)     = 0;
	virtual void WriteFieldEnd     ()                         = 0;
	virtual void WriteFieldStop    ()                         = 0;
	virtual void WriteMapBegin     (const TMap & map)         = 0;
	virtual void WriteMapEnd       ()                         = 0;
	virtual void WriteListBegin    (const TList & list)       = 0;
	virtual void WriteListEnd      ()                         = 0;
	virtual void WriteSetBegin     (const TSet & set)         = 0;
	virtual void WriteSetEnd       ()                         = 0;
	virtual void WriteBool         (bool b)                   = 0;
	virtual void WriteByte         (BYTE b)                   = 0;
	virtual void WriteI16          (__int16 i16)              = 0;
	virtual void WriteI32          (__int32 i32)              = 0;
	virtual void WriteI64          (__int64 i64)              = 0;
	virtual void WriteDouble       (double d)                 = 0;
	virtual void WriteString       (const TString & s)        = 0;
	virtual void WriteBinary       (const TBinary & b)        = 0;

	virtual void     ReadMessageBegin (TMessage & message) = 0;
	virtual void     ReadMessageEnd   ()                   = 0;
	virtual void     ReadStructBegin  (TStruct & struc)    = 0;
	virtual void     ReadStructEnd    ()                   = 0;
	virtual void     ReadFieldBegin   (TField & field)     = 0;
	virtual void     ReadFieldEnd     ()                   = 0;
	virtual void     ReadMapBegin     (TMap & map)         = 0;
	virtual void     ReadMapEnd       ()                   = 0;
	virtual void     ReadListBegin    (TList & list)       = 0;
	virtual void     ReadListEnd      ()                   = 0;
	virtual void     ReadSetBegin     (TSet & set)         = 0;
	virtual void     ReadSetEnd       ()                   = 0;
	virtual bool     ReadBool         ()                   = 0;
	virtual BYTE     ReadByte         ()                   = 0;
	virtual __int16  ReadI16          ()                   = 0;
	virtual __int32  ReadI32          ()                   = 0;
	virtual __int64  ReadI64          ()                   = 0;
	virtual double   ReadDouble       ()                   = 0;
	virtual void     ReadString       (TString & str)      = 0;
	virtual void     ReadBinary       (TBinary & bin)      = 0;
};

} // Protocol
} // Thrift
