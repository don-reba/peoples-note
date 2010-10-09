#pragma once

class IInkEditorView;
class INoteListView;

class InkEditorPresenter
{
private:

	IInkEditorView & inkEditorView;
	INoteListView  & noteListView;

public:

	InkEditorPresenter
		( IInkEditorView & inkEditorView
		, INoteListView  & noteListView
		);

private:

	void OnAccept();
	void OnCancel();
	void OnNewInkNote();
};
