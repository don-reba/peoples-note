#pragma once

#include "Note.h"

class EnNoteTranslator;
class IEditorView;
class INoteListModel;
class INoteListView;
class INoteView;
class IUserModel;

class EditorPresenter
{
private:

	IEditorView      & editorView;
	EnNoteTranslator & enNoteTranslator;
	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;

public:

	EditorPresenter
		( IEditorView      & editorView
		, INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, EnNoteTranslator & enNoteTranslator
		);

private:

	void OnAccept();
	void OnCancel();
	void OnEditNote();
	void OnNewNote();
};
