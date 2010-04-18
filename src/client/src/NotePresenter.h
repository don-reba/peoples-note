#pragma once

#include "EnNoteTranslator.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"

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

	void OnLoadingData
		( const wchar_t * uri
		, Blob          & blob
		);

	void OnOpenNote();
};
