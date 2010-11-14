#include "stdafx.h"
#include "MockPhotoEditorModel.h"

using namespace std;

void MockPhotoEditorModel::GetQuality(wstring & quality)
{
	quality = this->quality;
}

void MockPhotoEditorModel::GetResolution(wstring & resolution)
{
	resolution = this->resolution;
}

void MockPhotoEditorModel::SetQuality(const wstring & quality)
{
	this->quality = quality;
}

void MockPhotoEditorModel::SetResolution(const wstring & resolution)
{
	this->resolution = resolution;
}
