#include "stdafx.h"
#include "InkEditorModel.h"

#include "IRegistryKey.h"

InkEditorModel::InkEditorModel
	( IRegistryKey & registryKey
	 )
	 : registryKey (registryKey)
{
}

void InkEditorModel::GetPen
	( std::wstring & width
	, std::wstring & color
	)
{
	width = registryKey.GetString(L"pen width", L"1px");
	color = registryKey.GetString(L"pen color", L"black");
}

void InkEditorModel::SetPen
		( const wchar_t * width
		, const wchar_t * color
		)
{
	registryKey.SetString(L"pen width", width);
	registryKey.SetString(L"pen color", color);
}
