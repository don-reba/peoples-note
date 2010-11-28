#pragma once

#include "Guid.h"
#include "RecognitionEntry.h"

class EnRecognitionParser
{
public:

	void Parse
		( const std::wstring   & xml
		, RecognitionEntryList & entries
		, const Guid           & resource
		);
};
