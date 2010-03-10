#pragma once

#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"

class NotePresenter
{
private:

	INoteListView & noteListView;
	INoteView     & noteView;
	IUserModel    & userModel;

public:

	NotePresenter
		( INoteListView & noteListView
		, INoteView     & noteView
		, IUserModel    & userModel
		);

private:

	void OnOpenNote();

private:

	static std::wstring ProcessNote(std::wstring text);
};
