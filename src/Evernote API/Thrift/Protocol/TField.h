#pragma once

#include "TString.h"
#include "TType.h"

namespace Thrift
{
namespace Protocol
{

class TField
{
private:

	TString name;
	TType   type;
	short   id;

public:

	TField();

	const wchar_t * GetName() const;
	TType           GetType() const;
	short           GetID()   const;

	void SetName (const wchar_t * value);
	void SetType (TType value);
	void SetID   (short value);
};

} // Protocol
} // Thrift
