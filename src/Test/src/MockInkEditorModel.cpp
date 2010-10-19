#include "stdafx.h"
#include "MockInkEditorModel.h"

using namespace std;

void MockInkEditorModel::GetPen
	( wstring & width
	, wstring & color
	)
{
	width = penWidth;
	color = penColor;
}


void MockInkEditorModel::SetPen
	( const wchar_t * width
	, const wchar_t * color
	)
{
	penWidth = width;
	penColor = color;
}
