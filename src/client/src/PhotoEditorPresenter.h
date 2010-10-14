#pragma once

class INoteListModel;
class INoteListView;
class IPhotoEditorView;
class IUserModel;

class PhotoEditorPresenter
{
private:

	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	IPhotoEditorView & photoEditorView;
	IUserModel       & userModel;

public:

	PhotoEditorPresenter
		( INoteListModel   & noteListModel
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