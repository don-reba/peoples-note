#pragma once

class INoteListView;
class INoteView;
class IUserModel;
class EnNoteTranslator;

class NotePresenter
{
private:

	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;
	EnNoteTranslator & enNoteTranslator;

// interface

public:

	NotePresenter
		( INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, EnNoteTranslator & enNoteTranslator
		);

// event handlers

private:

	void OnCloseNote();
	void OnOpenNote();
};
