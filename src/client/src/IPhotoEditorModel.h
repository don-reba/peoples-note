#pragma once

class IPhotoEditorModel
{
public:

	virtual void GetQuality(std::wstring & quality) = 0;

	virtual void GetResolution(std::wstring & resolution) = 0;

	virtual void SetQuality(const std::wstring & quality) = 0;

	virtual void SetResolution(const std::wstring & resolution) = 0;
};