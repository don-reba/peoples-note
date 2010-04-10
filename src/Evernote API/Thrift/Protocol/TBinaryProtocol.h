#pragma once

#include "TProtocol.h"


namespace Thrift
{
namespace Transport
{
	class TTransport;
}

namespace Protocol
{

class TBinaryProtocol : public TProtocol
{
private:

	static const unsigned int versionMask = 0xFFFF0000;
	static const unsigned int version1    = 0x80010000;

	bool strictRead;
	bool strictWrite;

	int  readLength;
	bool checkReadLength;

	Transport::TTransport & trans;

public:

// interface

	TBinaryProtocol
		( Transport::TTransport & trans
		, bool strictRead  = false
		, bool strictWrite = true
		);

	virtual Transport::TTransport & GetTransport();

// write methods

	virtual void WriteMessageBegin (const TMessage & message);
	virtual void WriteMessageEnd   ();
	virtual void WriteStructBegin  (const TStruct & struc);
	virtual void WriteStructEnd    ();
	virtual void WriteFieldBegin   (const TField & field);
	virtual void WriteFieldEnd     ();
	virtual void WriteFieldStop    ();
	virtual void WriteMapBegin     (const TMap & map);
	virtual void WriteMapEnd       ();
	virtual void WriteListBegin    (const TList & list);
	virtual void WriteListEnd      ();
	virtual void WriteSetBegin     (const TSet & set);
	virtual void WriteSetEnd       ();
	virtual void WriteBool         (bool b);
	virtual void WriteByte         (BYTE b);
	virtual void WriteI16          (__int16 i16);
	virtual void WriteI32          (__int32 i32);
	virtual void WriteI64          (__int64 i64);
	virtual void WriteDouble       (double d);
	virtual void WriteString       (const TString & s);
	virtual void WriteBinary       (const TBinary & b);

// read methods

	virtual void     ReadMessageBegin (TMessage & message);
	virtual void     ReadMessageEnd   ();
	virtual void     ReadStructBegin  (TStruct & struc);
	virtual void     ReadStructEnd    ();
	virtual void     ReadFieldBegin   (TField & field);
	virtual void     ReadFieldEnd     ();
	virtual void     ReadMapBegin     (TMap & map);
	virtual void     ReadMapEnd       ();
	virtual void     ReadListBegin    (TList & list);
	virtual void     ReadListEnd      ();
	virtual void     ReadSetBegin     (TSet & set);
	virtual void     ReadSetEnd       ();
	virtual bool     ReadBool         ();
	virtual BYTE     ReadByte         ();
	virtual __int16  ReadI16          ();
	virtual __int32  ReadI32          ();
	virtual __int64  ReadI64          ();
	virtual double   ReadDouble       ();
	virtual void     ReadString       (TString & str);
	virtual void     ReadBinary       (TBinary & bin);

private:

	void CheckReadLength(int length);
	
	void ReadStringBody(int size, TString & str);

	int ReadAll(TBinary & bin, int off, int len);

	void DecodeUtf8(const TBinary & bin, TString & str);

	void EncodeUtf8(const TString & str, TBinary & bin);

};

} // Protocol
} // Thrift
