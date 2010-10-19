#pragma once
#include "IInkEditorModel.h"

class MockInkEditorModel : public IInkEditorModel
{
public:

	std::wstring penColor;
	std::wstring penWidth;

public:

	virtual void GetPen
		( std::wstring & width
		, std::wstring & color
		);

	virtual void SetPen
		( const wchar_t * width
		, const wchar_t * color
		);
};
