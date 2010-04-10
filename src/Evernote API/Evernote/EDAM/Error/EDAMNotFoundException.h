#pragma once

/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <windows.h>
#include <Thrift/Thrift.h>
#include <Thrift/Protocol.h>
#include <Thrift/Transport.h>


namespace Evernote
{
namespace EDAM
{
namespace Error
{

class EDAMNotFoundException : public Thrift::TException
{
public:

	struct Isset
	{
		bool identifier;
		bool key;
	};

	Thrift::Protocol::TString identifier;
	Thrift::Protocol::TString key;

	Isset __isset;

	EDAMNotFoundException();
	virtual ~EDAMNotFoundException() throw() {}

	void Read(Thrift::Protocol::TProtocol & iprot);
	void Write(Thrift::Protocol::TProtocol & oprot);
};
} // Error
} // EDAM
} // Evernote