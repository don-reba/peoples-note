#include "stdafx.h"
#include "PhotoEditorModel.h"

#include "IRegistryKey.h"

using namespace std;

PhotoEditorModel::PhotoEditorModel(IRegistryKey & registryKey)
	: registryKey (registryKey)
{
}

void PhotoEditorModel::GetQuality(wstring & quality)
{
	quality = registryKey.GetString(L"photo quality", L"");
}

void PhotoEditorModel::GetResolution(wstring & resolution)
{
	resolution = registryKey.GetString(L"photo resolution", L"");
}

void PhotoEditorModel::SetQuality(const wstring & quality)
{
	registryKey.SetString(L"photo quality", quality.c_str());
}

void PhotoEditorModel::SetResolution(const wstring & resolution)
{
	registryKey.SetString(L"photo resolution", resolution.c_str());
}
