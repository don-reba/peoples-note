#include "stdafx.h"
#include "NoteTagListPresenter.h"

#include "INoteTagListModel.h"
#include "INoteTagListView.h"
#include "INoteView.h"
#include "IUserModel.h"

using namespace boost;
using namespace std;

NoteTagListPresenter::NoteTagListPresenter
	( INoteTagListModel & noteTagListModel
	, INoteTagListView  & noteTagListView
	, INoteView         & noteView
	, IUserModel        & userModel
	)
	: noteTagListModel (noteTagListModel)
	, noteTagListView  (noteTagListView)
	, noteView         (noteView)
	, userModel        (userModel)
{
	noteTagListModel.ConnectTagsChanged (bind(&NoteTagListPresenter::OnTagsChanged, this));

	noteTagListView.ConnectCancel      (bind(&NoteTagListPresenter::OnCancel,      this));
	noteTagListView.ConnectOk          (bind(&NoteTagListPresenter::OnOk,          this));
	noteTagListView.ConnectTagCleared  (bind(&NoteTagListPresenter::OnTagCleared,  this));
	noteTagListView.ConnectTagSelected (bind(&NoteTagListPresenter::OnTagSelected, this));

	noteView.ConnectEditTags(bind(&NoteTagListPresenter::OnEditTags, this));
}

//---------------
// event handlers
//---------------

void NoteTagListPresenter::OnCancel()
{
	CloseView();
}

void NoteTagListPresenter::OnEditTags()
{
	OpenView();
}

void NoteTagListPresenter::OnOk()
{
	UpdateNote();
	CloseView();
}

void NoteTagListPresenter::OnTagCleared()
{
	ClearActiveTag();
}

void NoteTagListPresenter::OnTagsChanged()
{
	UpdateView();
}

void NoteTagListPresenter::OnTagSelected()
{
	SelectActiveTag();
}

//------------------
// utility functions
//------------------

void NoteTagListPresenter::ClearActiveTag()
{
	noteTagListModel.ClearTag(noteTagListView.GetActiveTag());
}

void NoteTagListPresenter::CloseView()
{
	noteTagListView.Hide();
}

void NoteTagListPresenter::OpenView()
{
	TagList tags;
	userModel.GetTags(tags);

	Note note;
	noteView.GetNote(note);

	TagList noteTags;
	userModel.GetNoteTags(note, noteTags);

	noteTagListModel.SetTags(tags, noteTags);

	noteTagListView.Show();
}

void NoteTagListPresenter::SelectActiveTag()
{
	noteTagListModel.SelectTag(noteTagListView.GetActiveTag());
}

void NoteTagListPresenter::UpdateNote()
{
	if (!noteTagListModel.IsDirty())
		return;

	Note note;
	noteView.GetNote(note);

	userModel.DeleteNoteTags(note.guid);
	foreach (const Tag & tag, noteTagListModel.GetSelectedTags())
		userModel.AddTagToNote(tag.name, note);
}

void NoteTagListPresenter::UpdateView()
{
	noteTagListView.SetTags
		( noteTagListModel.GetClearedTags()
		, noteTagListModel.GetSelectedTags()
		);
}
