#pragma once

#include "InkPen.h"

class IInkEditorModel;
class IInkEditorView;
class INoteListModel;
class INoteListView;
class IUserModel;

class InkEditorPresenter
{
private:

	IInkEditorModel & inkEditorModel;
	IInkEditorView  & inkEditorView;
	INoteListModel  & noteListModel;
	INoteListView   & noteListView;
	IUserModel      & userModel;

public:

	InkEditorPresenter
		( IInkEditorModel & inkEditorModel
		, IInkEditorView  & inkEditorView
		, INoteListModel  & noteListModel
		, INoteListView   & noteListView
		, IUserModel      & userModel
		);

// event handlers

private:

	void OnAccept();
	void OnCancel();
	void OnNewInkNote();
	void OnPenChanged();

// utility functions

private:

	InkPenColor GetPenColor(const wchar_t * name);
	InkPenWidth GetPenWidth(const wchar_t * name);

	const wchar_t * GetPenColorName(InkPenColor color);
	const wchar_t * GetPenWidthName(InkPenWidth width);
};
