#pragma once

#include "TString.h"

namespace Thrift
{
namespace Protocol
{

class TStruct
{
private:

	TString name;

public:

	TStruct();

	const wchar_t * GetName() const;

	void SetName(const wchar_t * value);
};

} // Protocol
} // Thrift
