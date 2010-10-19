#pragma once
#include "IInkEditormodel.h"

class IRegistryKey;

class InkEditorModel : public IInkEditorModel
{
private:

	IRegistryKey & registryKey;

public:

	InkEditorModel
		( IRegistryKey & registryKey
		);

	virtual void GetPen
		( std::wstring & width
		, std::wstring & color
		);

	virtual void SetPen
		( const wchar_t * width
		, const wchar_t * color
		);
};
