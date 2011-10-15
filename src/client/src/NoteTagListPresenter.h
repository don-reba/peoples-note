#pragma once

class INoteTagListModel;
class INoteTagListView;
class INoteView;
class IUserModel;

class NoteTagListPresenter
{
private:

	INoteTagListModel & noteTagListModel;
	INoteTagListView  & noteTagListView;
	INoteView         & noteView;
	IUserModel        & userModel;

public:

	NoteTagListPresenter
		( INoteTagListModel & noteTagListModel
		, INoteTagListView  & noteTagListView
		, INoteView         & noteView
		, IUserModel        & userModel
		);

// event handlers

private:

	void OnCancel();
	void OnClearedTag();
	void OnEditTags();
	void OnOk();
	void OnSelectedTag();
	void OnTagsChanged();

// utility functions

private:

	void ClearActiveTag();
	void CloseView();
	void OpenView();
	void SelectActiveTag();
	void UpdateNote();
	void UpdateView();
};
