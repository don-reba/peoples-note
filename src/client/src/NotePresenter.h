#pragma once

class INoteListModel;
class INoteListView;
class INoteView;
class IUserModel;
class EnNoteTranslator;

class NotePresenter
{
private:

	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;
	EnNoteTranslator & enNoteTranslator;

// interface

public:

	NotePresenter
		( INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, EnNoteTranslator & enNoteTranslator
		);

// event handlers

private:

	void OnCloseNote();
	void OnOpenNote();
};
