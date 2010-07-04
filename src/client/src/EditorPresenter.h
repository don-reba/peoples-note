#pragma once

class EnNoteTranslator;
class IEditorView;
class INoteListView;
class INoteView;
class IUserModel;

class EditorPresenter
{
private:

	IEditorView      & editorView;
	EnNoteTranslator & enNoteTranslator;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;

public:

	EditorPresenter
		( IEditorView      & editorView
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
