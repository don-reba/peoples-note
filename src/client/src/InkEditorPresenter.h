#pragma once

class IInkEditorView;
class INoteListModel;
class INoteListView;
class IUserModel;

class InkEditorPresenter
{
private:

	IInkEditorView & inkEditorView;
	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	IUserModel     & userModel;

public:

	InkEditorPresenter
		( IInkEditorView & inkEditorView
		, INoteListModel & noteListModel
		, INoteListView  & noteListView
		, IUserModel     & userModel
		);

private:

	void OnAccept();
	void OnCancel();
	void OnNewInkNote();
};
