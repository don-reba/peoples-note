#include "stdafx.h"
#include "InkEditorPresenter.h"

#include "IInkEditorView.h"
#include "INoteListView.h"

using namespace boost;

InkEditorPresenter::InkEditorPresenter
	( IInkEditorView & inkEditorView
	, INoteListView  & noteListView
	)
	: inkEditorView (inkEditorView)
	, noteListView  (noteListView)
{
	inkEditorView.ConnectAccept    (bind(&InkEditorPresenter::OnAccept,     this));
	inkEditorView.ConnectCancel    (bind(&InkEditorPresenter::OnCancel,     this));
	noteListView.ConnectNewInkNote (bind(&InkEditorPresenter::OnNewInkNote, this));
}

void InkEditorPresenter::OnAccept()
{
	inkEditorView.Hide();
}

void InkEditorPresenter::OnCancel()
{
	inkEditorView.Hide();
}

void InkEditorPresenter::OnNewInkNote()
{
	inkEditorView.Show();
}
