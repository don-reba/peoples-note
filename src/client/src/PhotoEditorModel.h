#pragma once
#include "IPhotoEditorModel.h"

class IRegistryKey;

class PhotoEditorModel : public IPhotoEditorModel
{
private:

	IRegistryKey & registryKey;

public:

	PhotoEditorModel(IRegistryKey & registryKey);

public:

	virtual void GetQuality(std::wstring & quality);

	virtual void GetResolution(std::wstring & resolution);

	virtual void SetQuality(const std::wstring & quality);

	virtual void SetResolution(const std::wstring & resolution);
};
