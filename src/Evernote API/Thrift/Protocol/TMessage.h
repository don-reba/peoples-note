#pragma once

#include "TMessageType.h"
#include "TString.h"

namespace Thrift
{
namespace Protocol
{

class TMessage
{
private:

	TString      name;
	TMessageType type;
	int          seqID;

public:

	TMessage();

	TMessage
		( const wchar_t      * name
		,       TMessageType   type
		,       int            seqID
		);

	const wchar_t * GetName()  const;
	TMessageType    GetType()  const ;
	int             GetSeqID() const ;

	void SetName  (const wchar_t * value);
	void SetType  (TMessageType value);
	void SetSeqID (int value);
};

} // Protocol
} // Thrift
