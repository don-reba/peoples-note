#include "stdafx.h"
#include "NotePresenter.h"

#include "Guid.h"

using namespace boost;
using namespace std;

NotePresenter::NotePresenter
	( INoteListView & noteListView
	, INoteView     & noteView
	, IUserModel    & userModel
	)
	: noteListView (noteListView)
	, noteView     (noteView)
	, userModel    (userModel)
{
	noteListView.ConnectOpenNote(bind(&NotePresenter::OnOpenNote, this));
}

void NotePresenter::OnOpenNote()
{
	Guid guid(noteListView.GetSelectedNoteGuid());
	noteView.SetBody(ProcessNote(userModel.GetNoteBody(guid)));
}

wstring NotePresenter::ProcessNote(wstring text)
{
	return text;
}
