#pragma once

class IFile;
class INoteListModel;
class INoteListView;
class IPhotoEditorView;
class IUserModel;

class PhotoEditorPresenter
{
private:

	IFile            & file;
	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	IPhotoEditorView & photoEditorView;
	IUserModel       & userModel;

public:

	PhotoEditorPresenter
		( IFile            & file
		, INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, IPhotoEditorView & photoEditorView
		, IUserModel       & userModel
		);

private:

	void OnCancel();
	void OnCapture();
	void OnNewPhotoNote();
	void OnOk();
};