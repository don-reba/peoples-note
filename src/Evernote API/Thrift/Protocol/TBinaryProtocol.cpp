#include "stdafx.h"

#include "TBinaryProtocol.h"
#include "TProtocolException.h"

#include <sstream>

//----------
// interface
//----------

using namespace Thrift;
using namespace Thrift::Protocol;
using namespace Thrift::Transport;

TBinaryProtocol::TBinaryProtocol
	( TTransport & trans
	, bool         strictRead
	, bool         strictWrite
	)
	: checkReadLength (false)
	, strictRead      (strictRead)
	, strictWrite     (strictWrite)
	, trans           (trans)
{
}

TTransport & TBinaryProtocol::GetTransport()
{
	return trans;
}

//--------------
// Write methods
//--------------

void TBinaryProtocol::WriteMessageBegin(const TMessage & message)
{
	if (strictWrite)
	{
		unsigned int version = version1 | message.GetType();
		WriteI32(version);
		WriteString(message.GetName());
		WriteI32(message.GetSeqID());
	}
	else
	{
		WriteString(message.GetName());
		WriteByte(message.GetType());
		WriteI32(message.GetSeqID());
	}
}

void TBinaryProtocol::WriteMessageEnd()
{
}

void TBinaryProtocol::WriteStructBegin(const TStruct & struc)
{
}

void TBinaryProtocol::WriteStructEnd()
{
}

void TBinaryProtocol::WriteFieldBegin(const TField & field)
{
	WriteByte(field.GetType());
	WriteI16(field.GetID());
}

void TBinaryProtocol::WriteFieldEnd()
{
}

void TBinaryProtocol::WriteFieldStop()
{
	WriteByte(TypeStop);
}

void TBinaryProtocol::WriteMapBegin(const TMap & map)
{
	WriteByte(map.GetKeyType());
	WriteByte(map.GetValueType());
	WriteI32(map.GetCount());
}

void TBinaryProtocol::WriteMapEnd()
{
}

void TBinaryProtocol::WriteListBegin(const TList & list)
{
	WriteByte(list.GetElementType());
	WriteI32(list.GetCount());
}

void TBinaryProtocol::WriteListEnd()
{
}

void TBinaryProtocol::WriteSetBegin(const TSet & set)
{
	WriteByte(set.GetElementType());
	WriteI32(set.GetCount());
}

void TBinaryProtocol::WriteSetEnd()
{
}

void TBinaryProtocol::WriteBool(bool b)
{
	WriteByte(b ? 1 : 0);
}

void TBinaryProtocol::WriteByte(BYTE b)
{
	static TBinary bin(1);
	bin[0] = b;
	trans.Write(bin, 0, 1);
}

void TBinaryProtocol::WriteI16(__int16 i16)
{
	static TBinary bin(2);
	bin[0] = 0xFF & (i16 >> 0x08);
	bin[1] = 0xFF & (i16 >> 0x00);
	trans.Write(bin, 0, 2);
}

void TBinaryProtocol::WriteI32(__int32 i32)
{
	static TBinary bin(4);
	bin[0] = 0xFF & (i32 >> 0x18);
	bin[1] = 0xFF & (i32 >> 0x10);
	bin[2] = 0xFF & (i32 >> 0x08);
	bin[3] = 0xFF & (i32 >> 0x00);
	trans.Write(bin, 0, 4);
}

void TBinaryProtocol::WriteI64(__int64 i64)
{
	static TBinary bin(8);
	bin[0] = 0xFF & (i64 >> 0x38);
	bin[1] = 0xFF & (i64 >> 0x30);
	bin[2] = 0xFF & (i64 >> 0x28);
	bin[3] = 0xFF & (i64 >> 0x20);
	bin[4] = 0xFF & (i64 >> 0x18);
	bin[5] = 0xFF & (i64 >> 0x10);
	bin[6] = 0xFF & (i64 >> 0x08);
	bin[7] = 0xFF & (i64 >> 0x00);
	trans.Write(bin, 0, 8);
}

void TBinaryProtocol::WriteDouble(double d)
{
	WriteI64(*reinterpret_cast<__int64*>(&d));
}

void TBinaryProtocol::WriteString(const TString & s)
{
	TBinary bin;
	EncodeUtf8(s, bin);
	WriteBinary(bin);
}

void TBinaryProtocol::WriteBinary(const TBinary & b)
{
	WriteI32(b.size());
	trans.Write(b, 0, b.size());
}

//-------------
// Read methods
//-------------

void TBinaryProtocol::ReadMessageBegin(TMessage & message)
{
	int size = ReadI32();
	if (size < 0)
	{
		unsigned int version = size & versionMask;
		if (version != version1)
		{
			std::wstringstream messageStream;
			messageStream << L"Bad version in ReadMessageBegin: " << version;

			throw TProtocolException
				(TProtocolException::BadVersion
				, messageStream.str().c_str()
				);
		}
		message.SetType(static_cast<TMessageType>(size & 0xff));
		TString name;
		ReadString(name);
		message.SetName(name.c_str());
		message.SetSeqID(ReadI32());
	}
	else
	{
		if (strictRead)
		{
			throw TProtocolException
				( TProtocolException::BadVersion
				, L"Missing version in readMessageBegin, old client?"
				);
		}
		TString name;
		ReadStringBody(size, name);
		message.SetName(name.c_str());
		message.SetType(static_cast<TMessageType>(ReadByte()));
		message.SetSeqID(ReadI32());
	}
}

void TBinaryProtocol::ReadMessageEnd()
{
}

void TBinaryProtocol::ReadStructBegin(TStruct & struc)
{
	struc.SetName(L"");
}

void TBinaryProtocol::ReadStructEnd()
{
}

void TBinaryProtocol::ReadFieldBegin(TField & field)
{
	TType type (static_cast<TType>(ReadByte()));
	short id   ((type == TypeStop) ? 0 : ReadI16());

	field.SetName(L"");
	field.SetType(type);
	field.SetID(id);
}

void TBinaryProtocol::ReadFieldEnd()
{
}

void TBinaryProtocol::ReadMapBegin(TMap & map)
{
	TType keyType   (static_cast<TType>(ReadByte()));
	TType valueType (static_cast<TType>(ReadByte()));
	int   count     (ReadI32());

	map.SetKeyType(keyType);
	map.SetValueType(valueType);
	map.SetCount(count);
}

void TBinaryProtocol::ReadMapEnd()
{
}

void TBinaryProtocol::ReadListBegin(TList & list)
{
	TType elementType (static_cast<TType>(ReadByte()));
	int   count       (ReadI32());

	list.SetElementType(elementType);
	list.SetCount(count);
}

void TBinaryProtocol::ReadListEnd()
{
}

void TBinaryProtocol::ReadSetBegin(TSet & set)
{
	TType elementType (static_cast<TType>(ReadByte()));
	int   count       (ReadI32());

	set.SetElementType(elementType);
	set.SetCount(count);
}

void TBinaryProtocol::ReadSetEnd()
{
}

bool TBinaryProtocol::ReadBool()
{
	return ReadByte() == 1;
}

BYTE TBinaryProtocol::ReadByte()
{
	static TBinary bin(1);
	ReadAll(bin, 0, 1);
	return bin[0];
}

__int16 TBinaryProtocol::ReadI16()
{
	static TBinary bin(2);
	ReadAll(bin, 0, 2);
	return
		( (static_cast<__int16>(bin[0]) << 0x8)
		| (static_cast<__int16>(bin[1]) << 0x0)
		);
}

__int32 TBinaryProtocol::ReadI32()
{
	static TBinary bin(4);
	ReadAll(bin, 0, 4);
	return
		( (static_cast<__int32>(bin[0]) << 0x18)
		| (static_cast<__int32>(bin[1]) << 0x10)
		| (static_cast<__int32>(bin[2]) << 0x08)
		| (static_cast<__int32>(bin[3]) << 0x00)
		);
}

__int64 TBinaryProtocol::ReadI64()
{
	static TBinary bin(8);
	ReadAll(bin, 0, 8);
	return
		( (static_cast<__int64>(bin[0]) << 0x38)
		| (static_cast<__int64>(bin[1]) << 0x30)
		| (static_cast<__int64>(bin[2]) << 0x28)
		| (static_cast<__int64>(bin[3]) << 0x20)
		| (static_cast<__int64>(bin[4]) << 0x18)
		| (static_cast<__int64>(bin[5]) << 0x10)
		| (static_cast<__int64>(bin[6]) << 0x08)
		| (static_cast<__int64>(bin[7]) << 0x00)
		);
}

double TBinaryProtocol::ReadDouble()
{
	__int64 n(ReadI64());
	return *reinterpret_cast<double*>(&n);
}

void TBinaryProtocol::ReadString(TString & str)
{
	TBinary bin;
	ReadBinary(bin);
	DecodeUtf8(bin, str);
}

void TBinaryProtocol::ReadBinary(TBinary & bin)
{
	int size(ReadI32());
	CheckReadLength(size);
	bin.resize(size);
	trans.ReadAll(bin, 0, size);
}

//------------------
// utility functions
//------------------

void TBinaryProtocol::CheckReadLength(int length)
{
	if (!checkReadLength)
		return;
	readLength -= length;
	if (readLength < 0)
	{
		std::wstringstream messageStream;
		messageStream << L"Message length exceeded: " << length;
		throw TProtocolException
			( TProtocolException::Unknown
			, messageStream.str().c_str()
			);
	}
}

void TBinaryProtocol::ReadStringBody(int size, TString & str)
{
	CheckReadLength(size);
	TBinary bin(size);
	ReadAll(bin, 0, size);
	return DecodeUtf8(bin, str);
}

int TBinaryProtocol::ReadAll(TBinary & bin, int off, int len)
{
	return trans.ReadAll(bin, off, len);
}

void TBinaryProtocol::DecodeUtf8(const TBinary & bin, TString & str)
{
	if (bin.empty())
	{
		str.clear();
		return;
	}

	LPCSTR cStr = reinterpret_cast<LPCSTR>(&bin[0]);
	int length = bin.size();

	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = MultiByteToWideChar
		( codePage // CodePage
		, flags    // dwFlags
		, cStr     // lpMultiByteStr
		, length   // cbMultiByte
		, NULL     // lpWideCharStr
		, 0        // cchWideChar
		);
	std::vector<wchar_t> result(resultSize + 1);
	MultiByteToWideChar
		( codePage   // CodePage
		, flags      // dwFlags
		, cStr       // lpMultiByteStr
		, length     // cbMultiByte
		, &result[0] // lpWideCharStr
		, resultSize // cchWideChar
		);
	str = &result[0];
}

void TBinaryProtocol::EncodeUtf8(const TString & str, TBinary & bin)
{
	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = WideCharToMultiByte
                ( codePage     // CodePage
                , flags        // dwFlags
                , str.c_str()  // lpWideCharStr
                , str.length() // cchWideChar
                , NULL         // lpMultiByteStr
                , 0            // cbMultiByte
                , NULL         // lpDefaultChar
                , NULL         // lpUsedDefaultChar
		);
	bin.resize(resultSize + 1);
	LPSTR resultStr = reinterpret_cast<char*>(&bin[0]);
	WideCharToMultiByte
                ( codePage     // CodePage
                , flags        // dwFlags
                , str.c_str()  // lpWideCharStr
                , str.length() // cchWideChar
                , resultStr    // lpMultiByteStr
                , resultSize   // cbMultiByte
                , NULL         // lpDefaultChar
                , NULL         // lpUsedDefaultChar
		);
	bin.resize(bin.size() - 1); // cut terminating zero
}
