#pragma once

#include "PhotoSettings.h"

class IFile;
class INoteListModel;
class INoteListView;
class IPhotoEditorModel;
class IPhotoEditorView;
class IUserModel;

class PhotoEditorPresenter
{
private:

	IFile             & file;
	INoteListModel    & noteListModel;
	INoteListView     & noteListView;
	IPhotoEditorModel & photoEditorModel;
	IPhotoEditorView  & photoEditorView;
	IUserModel        & userModel;

public:

	PhotoEditorPresenter
		( IFile             & file
		, INoteListModel    & noteListModel
		, INoteListView     & noteListView
		, IPhotoEditorModel & photoEditorModel
		, IPhotoEditorView  & photoEditorView
		, IUserModel        & userModel
		);

private:

	void OnCancel();
	void OnCapture();
	void OnNewPhotoNote();
	void OnOk();

private:

	void CreatePhotoNote(const std::wstring & imagePath);

	PhotoQuality    GetQuality    (const wchar_t * quality);
	PhotoResolution GetResolution (const wchar_t * resolution);

	const wchar_t * GetQualityName    (PhotoQuality quality);
	const wchar_t * GetResolutionName (PhotoResolution resolution);

	int GetPhotoHeight  (PhotoResolution resolution);
	int GetPhotoQuality (PhotoQuality    quality);
	int GetPhotoWidth   (PhotoResolution resolution);
};