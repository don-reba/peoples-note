#pragma once

#include "InkPen.h"

class IInkEditorModel
{
public:

	virtual void GetPen
		( std::wstring & width
		, std::wstring & color
		) = 0;

	virtual void SetPen
		( const wchar_t * width
		, const wchar_t * color
		) = 0;
};
