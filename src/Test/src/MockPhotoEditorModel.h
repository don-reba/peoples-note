#pragma once
#include "IPhotoEditorModel.h"

class MockPhotoEditorModel : public IPhotoEditorModel
{
public:

	std::wstring quality;
	std::wstring resolution;

public:

	virtual void GetQuality(std::wstring & quality);

	virtual void GetResolution(std::wstring & resolution);

	virtual void SetQuality(const std::wstring & quality);

	virtual void SetResolution(const std::wstring & resolution);
};
