#pragma once

#include "Protocol/TProtocol.h"

namespace Thrift
{

class TProcessor
{
public:

	virtual bool Process
		( Protocol::TProtocol & iprot
		, Protocol::TProtocol & oprot
		) = 0;
};

} // Thrift
